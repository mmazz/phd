import m5
from m5.objects import *
from gem5.runtime import get_runtime_isa
m5.util.addToPath("../../")
from caches import *
from common import SimpleOpts
thispath = os.path.dirname(os.path.realpath(__file__))
default_binary = os.path.join(
    thispath,
    "../../../",
    "tests/test-progs/hello/hello-std",
    #"tests/test-progs/hello/bin/x86/linux/hello",
)
SimpleOpts.add_option("binary", nargs="?", default=default_binary)
args = SimpleOpts.parse_args()

num_cpus = 2

system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = "1GHz"
system.clk_domain.voltage_domain = VoltageDomain()
system.mem_mode = "timing"  # Use timing accesses
system.mem_ranges = [AddrRange("512MB")]  # Create an address range
cpus = []
for i in range(num_cpus):
    cpu = X86TimingSimpleCPU(cpu_id=i)
    cpus.append(cpu)

system.membus = NoncoherentXBar(forward_latency=10, frontend_latency=5,  response_latency=100, width=64)
system.l2bus = L2XBar()
system.l2cache = L2Cache(args)
system.l2cache.connectCPUSideBus(system.l2bus)
system.l2cache.connectMemSideBus(system.membus)

system.system_port = system.membus.cpu_side_ports
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports
system.workload = SEWorkload.init_compatible(args.binary)


process = Process(cmd=args.binary)
for cpu in cpus:
    cpu.clk_domain = system.clk_domain
    cpu.addPrivateSplitL1Caches(
        L1ICache(args),
        L1DCache(args)
    )
    cpu.icache.connectBus(system.l2bus)
    cpu.dcache.connectBus(system.l2bus)
    cpu.createInterruptController()
    cpu.interrupts[0].pio = system.membus.mem_side_ports
    cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
    cpu.interrupts[0].int_responder = system.membus.mem_side_ports
    cpu.createThreads()
# One of these two lines. The first one gives me the first error that mentioned
# and the second one the second...
    cpu.workload = [process] * int(num_cpus)
    cpu.workload = process

system.cpu = cpus
root = Root(full_system=False, system=system)
m5.instantiate()
print("Beginning simulation!")
exit_event = m5.simulate()
print("Exiting @ tick %i because %s" % (m5.curTick(), exit_event.getCause()))
