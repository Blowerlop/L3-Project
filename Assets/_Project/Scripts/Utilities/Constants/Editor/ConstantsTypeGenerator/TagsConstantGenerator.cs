using System.CodeDom.Compiler;
using Project.Extensions;

namespace _Project.ConstantGenerator
{
    public class TagsConstantsGeneratorGenerator : IConstantGenerator
    {
        public string className { get; set; } = "Tags";
        
        
        public void Write(IndentedTextWriter streamWriter)
        {
            foreach (string tag in GetTags())
            {
                ConstantsGenerator.WriteSummary(streamWriter, tag);
                streamWriter.WriteLine($"public const string {tag.ConvertToValidIdentifier()} = \"{tag}\";");
            }
        }

        private string[] GetTags()
        {
            return UnityEditorInternal.InternalEditorUtility.tags;
        }
    }
}