using Mewlist.Weaver;

namespace _Project._200_Dev.ParrelSync.IL_Weaving
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
