// #if UNITY_EDITOR
// using Mewlist.Weaver;
// using Unity.Netcode;
//
// namespace Project
// {
//     public class ServerRpcWeaver : IWeaver
//     {
//         public void Weave(AssemblyInjector assemblyInjector)
//         {
//             assemblyInjector
//                 .OnMainAssembly()
//                 .OnAttribute<ServerRpcAttribute>()
//                 .Do(new ServerRpcILInjector())
//                 .Inject();
//         }
//     }
// }
// #endif