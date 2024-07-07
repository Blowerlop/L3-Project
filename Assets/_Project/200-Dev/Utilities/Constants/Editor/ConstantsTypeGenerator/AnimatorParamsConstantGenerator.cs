using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Linq;
using Project.Extensions;
using UnityEditor;
using UnityEditor.Animations;
using UnityEngine;

namespace _Project.ConstantGenerator
{
    public class AnimatorParamsConstantGenerator : IConstantGenerator
    {
        public string className { get; set; } = "AnimatorsParam";

        private readonly string[] _paramsToExclude =
        {
            "AnimSpeed",
            "Start",
            "Normal",
            "Highlighted"
        };
        
        
        public void Write(IndentedTextWriter streamWriter)
        {
            foreach (AnimatorControllerParameter exposedParameter in GetParameters())
            {
                ConstantsGenerator.WriteSummary(streamWriter, $"{exposedParameter.name} : {exposedParameter.type}");
                streamWriter.WriteLine($"public const int {exposedParameter.name.ConvertToValidIdentifier()} = {exposedParameter.nameHash};");
            }
        }
        
        private AnimatorControllerParameter[] GetParameters()
        {
            List<AnimatorControllerParameter> animatorControllerParameters = new List<AnimatorControllerParameter>();
            
            string[] guids = AssetDatabase.FindAssets("t:AnimatorController");
            
            foreach (string guid in guids)
            {
                string assetPath = AssetDatabase.GUIDToAssetPath(guid);
                AnimatorController animator = AssetDatabase.LoadAssetAtPath<AnimatorController>(assetPath);
                animatorControllerParameters.AddRange(animator.parameters);
            }

            return animatorControllerParameters.Where(x => _paramsToExclude.Contains(x.name) == false).DistinctBy(x => x.name).ToArray();
        }
    }
}