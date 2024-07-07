using System;
using System.Linq;
using System.Reflection;
using Mewlist.Weaver;
using Mono.Cecil;
using Mono.Cecil.Cil;
using Sirenix.Utilities;
using UnityEngine;

namespace _Project._200_Dev.Networking.IL_Weaving.Server
{
    public class ServerILInjector : IILInjector
    {
        public void Validate(ICustomAttribute customAttribute)
        {
        } 

        public void Inject(CustomAttribute customAttribute, ModuleDefinition moduleDefinition, MethodDefinition methodDefinition)
        {
            methodDefinition.CustomAttributes.Remove(customAttribute);

            Type serverMethodsType = typeof(ServerMethods);
            MethodReference singletonRef = moduleDefinition.ImportReference(serverMethodsType.GetMethod("IsNetworkManagerSingletonExist")); 
            MethodReference isListeningRef = moduleDefinition.ImportReference(serverMethodsType.GetMethod("IsListening"));
            MethodReference isServerRef = moduleDefinition.ImportReference(serverMethodsType.GetMethod("IsServer")); 
            
            Type debugType = typeof(Debug);
            MethodReference logErrorRef = moduleDefinition.ImportReference(debugType.GetMethod("LogError", new []{ typeof(object) }));
            
            TypeReference returnTypeRef = methodDefinition.ReturnType;
            Type type = Type.GetType(returnTypeRef.FullName);
            if (type == null)
            {
                Debug.Log($"[{this}] Type not found for {returnTypeRef.FullName}");
                return;
            }
            MethodReference returnTypeCtorRef = null;
            if (type.IsNullableType() == false && type.FullName != "System.Void" && type.FullName != "System.Boolean")
            {
                ConstructorInfo returnTypeCtor = type.GetConstructors().FirstOrDefault();
                returnTypeCtorRef = moduleDefinition.ImportReference(returnTypeCtor);
            }  
            
            ILProcessor processor = methodDefinition.Body.GetILProcessor(); 
            Instruction first = methodDefinition.Body.Instructions.First(); 
             
            
            // DEBUG
            // Instruction returnInstruction1 = Instruction.Create(OpCodes.Ldstr, "No singleton"); 
            // Instruction returnInstruction2 = Instruction.Create(OpCodes.Ldstr, "Not connected"); 
            
            // DEBUG
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Ldstr, "On commence")); 
            // processor.InsertBefore(first, Instruction.Create(OpCodes.Call, logErrorRef));

            Instruction returnConstructorInstruction;
            if (type.IsNullableType())
            {
                returnConstructorInstruction = Instruction.Create(OpCodes.Ldnull); 
            }
            else 
                returnConstructorInstruction = type.FullName switch
                {
                    "System.Boolean" => Instruction.Create(OpCodes.Ldc_I4_0), 
                    "System.Void" => Instruction.Create(OpCodes.Nop),
                    _ => Instruction.Create(OpCodes.Newobj, returnTypeCtorRef)
                };
 
            // If Singleton is null, return
            processor.InsertBefore(first, Instruction.Create(OpCodes.Call, singletonRef));  
            processor.InsertBefore(first, Instruction.Create(OpCodes.Brfalse_S, returnConstructorInstruction));
            
            // If IsListening false, return;
            processor.InsertBefore(first, Instruction.Create(OpCodes.Call, isListeningRef)); 
            processor.InsertBefore(first, Instruction.Create(OpCodes.Brfalse_S, returnConstructorInstruction)); 
            
            // If IsServer, execute normal method 
            processor.InsertBefore(first, Instruction.Create(OpCodes.Call, isServerRef)); 
            processor.InsertBefore(first, Instruction.Create(OpCodes.Brtrue_S, first)); 
            
            // All conditions pass, return and log error because client called this method
            processor.InsertBefore(first, Instruction.Create(OpCodes.Ldstr, "Only the server can invoke a Server method")); 
            processor.InsertBefore(first, Instruction.Create(OpCodes.Call, logErrorRef));
            
            processor.InsertBefore(first, returnConstructorInstruction);
            processor.InsertBefore(first, Instruction.Create(OpCodes.Ret));
        } 
    } 
}
