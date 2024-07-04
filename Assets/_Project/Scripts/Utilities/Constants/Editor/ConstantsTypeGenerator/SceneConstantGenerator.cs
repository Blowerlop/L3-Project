using System.CodeDom.Compiler;
using System.IO;
using Project.Extensions;
using UnityEditor;
using UnityEngine;

namespace _Project.ConstantGenerator
{
    public sealed class SceneConstantGenerator : IConstantGenerator
    {
        public string className { get; set; } = "Scenes";
        
        
        public void Write(IndentedTextWriter streamWriter)
        {
            EditorBuildSettingsScene[] scenes = GetScenes();
            for (int i = 0; i < scenes.Length; i++)
            {
                EditorBuildSettingsScene scene = scenes[i];

                ConstantsGenerator.WriteSummary(streamWriter, Path.GetFileNameWithoutExtension(scene.path));
                streamWriter.WriteLine($"public const int {scene.path.ConvertToValidIdentifier(true)} = {i};");
            }
        }
        private EditorBuildSettingsScene[] GetScenes()
        {
            return EditorBuildSettings.scenes;
        }
    }
}