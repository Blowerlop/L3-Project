#if UNITY_EDITOR
using System;
using System.Linq;
using Mewlist.Weaver;
using Mono.Cecil;
using Mono.Cecil.Cil;
using ParrelSync;
using OpCodes = Mono.Cecil.Cil.OpCodes;

namespace Project
{
    public class ParrelSyncILInjector : IILInjector
    {
        public void Validate(ICustomAttribute customAttribute)
        {
        }
        
        public void Inject(CustomAttribute customAttribute, ModuleDefinition moduleDefinition, MethodDefinition methodDefinition)
        {
            methodDefinition.CustomAttributes.Remove(customAttribute);
            
            Type clonesManagerType = typeof(ClonesManager);
            MethodReference isCloneRef = moduleDefinition.ImportReference(clonesManagerType.GetMethod("IsClone"));
            
            ILProcessor processor = methodDefinition.Body.GetILProcessor();
            Instruction first = methodDefinition.Body.Instructions.First();

            processor.InsertBefore(first, Instruction.Create(OpCodes.Call, isCloneRef));
            // If false, continue 
            processor.InsertBefore(first, Instruction.Create(OpCodes.Brfalse_S, first));
            // else return
            processor.InsertBefore(first, Instruction.Create(OpCodes.Ret)); 
        }    
    }
}
#endif