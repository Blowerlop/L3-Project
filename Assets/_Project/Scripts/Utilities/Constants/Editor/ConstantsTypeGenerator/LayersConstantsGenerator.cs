using System.CodeDom.Compiler;
using Project.Extensions;
using UnityEngine;

namespace _Project.ConstantGenerator
{
    public class LayersConstantsGenerator : IConstantGenerator
    {
        public string className { get; set; } = "Layers";
        
        
        public void Write(IndentedTextWriter streamWriter)
        {
            string[] layers = GetLayers();
            foreach (string layer in GetLayers())
            {
                ConstantsGenerator.WriteSummary(streamWriter, layer); 
                streamWriter.WriteLine($"public const int {layer.ConvertToValidIdentifier()}Mask = {LayerMask.GetMask(layer)};");
                streamWriter.WriteLine($"public const int {layer.ConvertToValidIdentifier()}Index = {LayerMask.NameToLayer(layer)};"); 
                streamWriter.WriteLine($"public const string {layer.ConvertToValidIdentifier()}Name = \"{layer}\";");
            }
        }

        private string[] GetLayers()
        {
            return UnityEditorInternal.InternalEditorUtility.layers;
        }
    }
}