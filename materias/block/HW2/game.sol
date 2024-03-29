// SPDX-License-Identifier: GPL-3.0
pragma solidity ^0.8.0;


contract CoinFlip
{
    bytes32 public constant HEAD = "HEAD";
    bytes32 public constant TAIL = "TAIL";

    uint public num_players = 0;
    address[2] public players;
    mapping(address => bytes32) public choices;
    mapping(address => bytes32) public choicesRevealed;

    mapping(address => uint256) private userBalances;


    // Pago inicial
    function deposit(uint256 deposito) payable public
    {
        // que no haya mas de 2 jugadores
        require(num_players < 2, "Max players, wait for one of them end playing");
        require(msg.value >= 100 gwei, "Not enough funds"); // fondos... 1 eth
        if(players[0]==address(0))
        {
            players[0]=msg.sender;
        }
        else
        {
            players[1]=msg.sender;
        }
        userBalances[msg.sender] += deposito;
        num_players += 1;
    }

    function balanceOf(address account) public view returns (uint256)
    {
        return userBalances[account];
    }

    function hashChoice(bytes32 choice, uint64 randomness) external pure returns(bytes32)
    {
        return keccak256(abi.encodePacked(choice, randomness));
    }

// sol reentrancy
    function withdrawBalance() public
    {
        // evito que un jugador pueda no revelar su opcion y el otro si y retirar si sabe que perdio.
        // basicamente se lo doy como perdida y retira. Pero le doy al otro usuario su ganancia
        if(msg.sender == players[0])
        {
            if(choicesRevealed[players[1]]!= bytes32(0))
            {
                userBalances[players[1]] += 200;
            }
        }
        if(msg.sender == players[1])
        {
            if(choicesRevealed[players[0]]!= bytes32(0))
            {
                userBalances[players[0]] += 200;
            }
        }
        uint amountToWithdraw = userBalances[msg.sender];
        userBalances[msg.sender] = 0;
        delete userBalances[msg.sender];
        num_players -= 1;
        // para evitar que uno revele su respuesta y el otro retire todo
        choicesRevealed[players[0]] = bytes32(0);
        choicesRevealed[players[1]] = bytes32(0);
        choices[players[0]] = bytes32(0);
        choices[players[1]] = bytes32(0);
        payable(msg.sender).transfer(amountToWithdraw);
    }

    function play(bytes32 choice) payable public
    {
        require(choices[msg.sender] == bytes32(0), "You had make your choice");
        require(userBalances[msg.sender] >= 100, "Not enough funds in the contract, make a new deposit");
        userBalances[msg.sender] -= 100;
        choices[msg.sender] = choice;
    }

    function evaluateChoice(bytes32 choice, uint64 randomness) external
    {
        // que los dos ya hallan elegido
        require(choices[players[0]] != bytes32(0), " Player 0 has not make a choice");
        require(choices[players[1]] != bytes32(0), " Player 1 has not make a choice");
        require(keccak256(abi.encodePacked(choice, randomness)) == choices[msg.sender], "Your choice and randomness do not correspond to your previus selection");
        choicesRevealed[msg.sender] = choice;
        // Si ya jugaron los dos directamente se ejecuta el evaluate
        if((msg.sender==players[0]) && (choicesRevealed[players[1]]!= bytes32(0)))
        {
            evaluate();
        }
        else if((msg.sender==players[1]) && (choicesRevealed[players[0]]!= bytes32(0)))
        {
            evaluate();
        }
    }

    function evaluate() internal
    {
        require(choicesRevealed[players[0]] != bytes32(0), "Player 0 has not revealed his choice");
        require(choicesRevealed[players[1]] != bytes32(0), "Player 1 has not revealed his choice");

        if(choicesRevealed[players[0]] == choicesRevealed[players[1]])
        {
            userBalances[players[1]] += 200;
            choicesRevealed[players[0]] = bytes32(0);
            choicesRevealed[players[1]] = bytes32(0);
            choices[players[0]] = bytes32(0);
            choices[players[1]] = bytes32(0);
        }
        else
        {
            userBalances[players[0]] += 200;
            choicesRevealed[players[0]] = bytes32(0);
            choicesRevealed[players[1]] = bytes32(0);
            choices[players[0]] = bytes32(0);
            choices[players[1]] = bytes32(0);
        }
    }
    receive() external payable {}
}
