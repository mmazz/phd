// SPDX-License-Identifier: GPL-3.0
pragma solidity ^0.8.0;


contract Token
{
    // a public payable address that defines the contract’s “owner,” that is, the
    // user that deploys the contract
    address public owner;
    string public symbol;
    string public name;
    uint8 public decimals;
    uint256 public _totalSupply = 0;
    uint256 public _maxTokens = 1_000_000;
    uint128 public price = 600;
    mapping(address => uint256) balances;



    // an event that contains two addresses and a uint256
    event Transfer(address indexed from, address indexed to, uint256 value);
    // an event that contains an address and a uint256
    // emitir seria
    event Mint(address indexed to,  uint256 value);

       // an event that contains an address and a uint256
    event Sell(address indexed from, uint256 value);


    // function that initializes the contract as needed
    constructor()
    {
        symbol = "CC";
        name = "Chiquito Coin";
        decimals = 18;
        owner = msg.sender;
      //  balances[owner] = _totalSupply;
        emit Transfer(address(0), owner, _totalSupply);
    }




    // a view function that returns a uint256 of the total amount of minted tokens
    function totalSupply() external view returns (uint256)
    {
        return _totalSupply;
    }

    // a view function returns a uint256 of the amount of tokens an address owns
    function balanceOf(address account) external view returns (uint256 balance)
    {
        return balances[account];
    }

    //a view function that returns a string with the token’s name
    function getName() public view returns (string memory)
    {
        return name;
    }

    // a view function that returns a string with the token’s symbol
    function getSymbol()  public view returns  (string memory)
    {
        return symbol;
    }

    // a view function that returns a uint128 with the token’s price (at which
    // users can redeem their tokens)
    function getPrice() public view returns  (uint128)
    {
        return price;
    }

    //a function that transfers value amount of tokens between the caller’s address and
    // the address to; if the transfer completes successfully, the function emits an event
    //  Transfer with the sender’s and receiver’s addresses and the amount of transferred
    // tokens and returns a boolean value (true)
    function transfer(address to, uint256 value)  external returns (bool success)
    {
        balances[msg.sender] = balances[msg.sender] - value;
        balances[to] = balances[to] + value;
        emit Transfer(msg.sender, to, value);
        return true;
    }

    // a function that enables only the owner to create value new tokens and give them
    // to address to; if the operation completes successfully, the function emits an
    // event Mint with the receiver’s address and the amount of minted tokens and returns
    // a boolean value (true)
    function mint(address to, uint256 value) public
    {
        require(msg.sender == owner, "Do not have privilages to mint");
        require(_totalSupply+value <= _maxTokens, "No more tokens to emit");
        _totalSupply += value;
        balances[to] = balances[to] + value;
        emit Mint( to, value);
    }

    // a function that enables a user to sell tokens for wei at a price of 600 wei per token;
    // if the operation completes successfully, the sold tokens are removed from the
    // circulating supply, and the function emits an event Sell with the seller’s address
    // and the amount of sold tokens and returns a boolean value (true)
    function sell(uint256 value) public payable returns(bool)
    {
        require(balances[msg.sender]>= value, "Not enough tokens");
        balances[msg.sender] -= value;
        _totalSupply -= value;
        payable(msg.sender).transfer(value*price);
        emit Sell(msg.sender, value);
        return true;
    }

    // a function that enables only the owner to destroy the contract; the
    // contract’s balance in wei, at the moment of destruction, should be transferred to
    // the owner’s address
    function close() public payable
    {
        require(msg.sender == owner, "Do not have privilages to close contract");
        payable(owner).transfer(_totalSupply*price);
    }

    //functions that enable anyone to send Ether to the contract’s account

}

