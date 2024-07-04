#if UNITY_EDITOR
using Mewlist.Weaver;

namespace Project
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
#endif