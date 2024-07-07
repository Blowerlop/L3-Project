// #if UNITY_EDITOR
// using System;
// using System.Linq;
// using System.Reflection;
// using Mewlist.Weaver;
// using Mono.Cecil;
// using Mono.Cecil.Cil;
// using Unity.Netcode;
// using UnityEngine;

// namespace Project
// {
    // public class ServerRpcILInjector : IILInjector
    // {
    //     public void Validate(ICustomAttribute customAttribute)
    //     {
    //     } 

        // Not working because the method can be called by the client safely but the moment it is called on the server trough the RPC it will throw the error. I can't tell if the method is called by the RPC or the server directly
        // public void Inject(CustomAttribute customAttribute, ModuleDefinition moduleDefinition, MethodDefinition methodDefinition)
        // {
            // Type networkManagerType = typeof(NetworkManager);
            // var singletonRef = moduleDefinition.ImportReference(networkManagerType.GetProperty("Singleton", BindingFlags.Static | BindingFlags.Public).GetGetMethod());
            // MethodReference isListeningRef = moduleDefinition.ImportReference(networkManagerType.GetMethod("get_IsListening"));
            // MethodReference isClientRef = moduleDefinition.ImportReference(networkManagerType.GetProperty("IsClient").GetGetMethod());
            //  
            // Type debugType = typeof(Debug); 
            // MethodReference logErrorRef = moduleDefinition.ImportReference(debugType.GetMethod("LogError", new []{ typeof(object) }));
            //
            // ILProcessor processor = methodDefinition.Body.GetILProcessor(); 
            // Instruction first = methodDefinition.Body.Instructions.First(); 
            //
            // Instruction _return = Instruction.Create(OpCodes.Ret);
            //
            // // If Singleton is null, return
            // // processor.InsertBefore(first, Instruction.Create(OpCodes.Call, singletonRef));
            // // processor.InsertBefore(first, Instruction.Create(OpCodes.Brfalse_S, _return)); 
            //
            // // If IsListening false, return;
            // // processor.InsertBefore(first, Instruction.Create(OpCodes.Call, isListeningRef)); 
            // // processor.InsertBefore(first, Instruction.Create(OpCodes.Brfalse_S, _return));  
            //
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Ldstr, "Call")); 
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Call, logErrorRef));
            //
            // // If IsClient, execute normal method 
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Call, singletonRef)); 
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Call, isClientRef)); 
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Brtrue_S, first)); 
            //
            // // All conditions pass, return and log error because server called this method
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Ldstr, "Only a client can invoke a ServerRpc method")); 
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Call, logErrorRef));
            //
            // processor.InsertBefore(first, _return);
        // }  
    // } 
// }
// #endif