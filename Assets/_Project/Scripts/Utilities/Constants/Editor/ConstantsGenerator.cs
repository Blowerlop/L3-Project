using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using UnityEditor;
using UnityEngine;

namespace _Project.ConstantGenerator
{
    public static class ConstantsGenerator
    {
        private const string _NAMESPACE = "Project";
        private const string _FILE_NAME = "Constants";
        
        
        [MenuItem("Tools/Generate Unity Constants", priority = 1000)]
        public static void Generate()
        {
            Debug.Log("Start generating constants...");
            
            
            string path = GetExistingFile();
            
            if (path == null)
            {
                string directory = EditorUtility.OpenFolderPanel($"Choose location for {_FILE_NAME}.cs", Application.dataPath, "");
                if (string.IsNullOrEmpty(directory))
                {
                    return;
                }
            
                path = Path.Combine(directory, $"{_FILE_NAME}.cs");
            }
        
            HashSet<IConstantGenerator> constantGenerators = new HashSet<IConstantGenerator>();
            foreach (Type type in TypeCache.GetTypesDerivedFrom<IConstantGenerator>())
            {
                if (Activator.CreateInstance(type) is IConstantGenerator constant) constantGenerators.Add(constant);
            }
        
            using (StreamWriter output = new StreamWriter(path)) 
            using (IndentedTextWriter streamWriter = new IndentedTextWriter(output))
            {
                streamWriter.WriteLine($"namespace {_NAMESPACE}");
                streamWriter.WriteLine("{");
                streamWriter.Indent++;
                streamWriter.WriteLine($"public static class {_FILE_NAME}");
                streamWriter.WriteLine("{");
                streamWriter.Indent++;
            
                foreach (IConstantGenerator constantGenerator in constantGenerators)
                {
                    streamWriter.WriteLine($"public static class {constantGenerator.className}");
                    streamWriter.WriteLine("{");
                    streamWriter.Indent++;
                
                    constantGenerator.Write(streamWriter);
                
                    streamWriter.Indent--;
                    streamWriter.WriteLine("}");
                    streamWriter.WriteLine();
                }
                
                streamWriter.Indent--;
                streamWriter.WriteLine("}");
                streamWriter.Indent--;
                streamWriter.WriteLine("}");
            }
            
            AssetDatabase.Refresh();
            
            Debug.Log("Constants generation finished !");
        }
    
        private static string GetExistingFile()
        {
            return Directory.GetFiles(Application.dataPath, "*.cs", SearchOption.AllDirectories)
                .FirstOrDefault(file => Path.GetFileNameWithoutExtension(file) == $"{_FILE_NAME}");
        }
        
        public static void WriteSummary(IndentedTextWriter streamWriter, string text)
        {
            streamWriter.WriteLine("/// <summary>");
            streamWriter.WriteLine($"/// {text}");
            streamWriter.WriteLine("/// </summary>");
        }
    }

    public interface IConstantGenerator
    {
        public string className { get; set; }
        public void Write(IndentedTextWriter streamWriter);
    }
}