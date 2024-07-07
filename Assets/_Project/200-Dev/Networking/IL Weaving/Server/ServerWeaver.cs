using Mewlist.Weaver;

namespace _Project._200_Dev.Networking.IL_Weaving.Server
{
    public class ServerWeaver : IWeaver
    {
        public void Weave(AssemblyInjector assemblyInjector)
        {
            assemblyInjector
                .OnMainAssembly()
                .OnAttribute<ServerAttribute>()
                .Do(new ServerILInjector())
                .Inject();
        }
    }
}
