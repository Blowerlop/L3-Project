#if UNITY_EDITOR
using Mewlist.Weaver;

namespace Project
{
    public class ParrelSyncWeaver : IWeaver
    {
        public void Weave(AssemblyInjector assemblyInjector)
        {
            assemblyInjector
                .OnMainAssembly()
                .OnAttribute<ParrelSyncIgnoreAttribute>()
                .Do(new ParrelSyncILInjector())
                .Inject();
        }
    }
}
#endif 