using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Project.Extensions;
using UnityEngine;

namespace _Project.ConstantGenerator
{
    public class ResourcesConstantsGenerator : IConstantGenerator
    {
        private class Folder
        {
            public string directoryFullPath;
            public string directoryLocalPath;
            public readonly List<Folder> additionalFolders = new List<Folder>();
            public string[] files = Array.Empty<string>();

            public void WriteBase(IndentedTextWriter streamWriter)
            {
                foreach (Folder folder in additionalFolders)
                {
                    folder.Write(streamWriter);
                }
                
                foreach (string file in files)
                {
                    string filePath = $"{file}";
                    
                    ConstantsGenerator.WriteSummary(streamWriter, filePath);
                    streamWriter.WriteLine($"public const string {file.ConvertToValidIdentifier()} = \"{filePath.Replace(@"\", "/")}\";");
                }
            }

            private void Write(IndentedTextWriter streamWriter)
            {
                streamWriter.WriteLine($"public static class {directoryLocalPath}");
                streamWriter.WriteLine("{");
                streamWriter.Indent++;
                
                foreach (var folder in additionalFolders)
                {
                    folder.Write(streamWriter);
                }
                
                string directoryFullLocalPath = directoryFullPath.Split(@"Resources\", 2).Last();
                foreach (string file in files)
                {
                    string filePath = $"{directoryFullLocalPath}/{file}";
                    
                    ConstantsGenerator.WriteSummary(streamWriter, filePath);
                    streamWriter.WriteLine($"public const string {file.ConvertToValidIdentifier()} = \"{filePath.Replace(@"\", "/")}\";");
                }
                
                streamWriter.Indent--;
                streamWriter.WriteLine("}");
                streamWriter.WriteLine();
            }
        }
        
        public string className { get; set; } = "Resources";

        private static readonly string ResourcesDirectoryPath = Application.dataPath + "/Resources";


        public void Write(IndentedTextWriter streamWriter)
        {
            Folder folder = GetResources();
            folder.WriteBase(streamWriter);
        }

        private Folder GetResources()
        {
            Folder folder = new Folder { directoryFullPath = ResourcesDirectoryPath, directoryLocalPath = "Resources" };
            PopulateDirectoriesFiles(folder);
            PopulateFolder(folder);
            return folder;
        }

        private void PopulateFolder(Folder folder)
        {
            string[] directories = Directory.GetDirectories(folder.directoryFullPath);
            foreach (string directory in directories)
            {
                string directoryLocalPath = directory.Split($@"{folder.directoryLocalPath}\", 2).Last();
                Folder newFolder = new Folder { directoryFullPath = directory, directoryLocalPath = directoryLocalPath };
                folder.additionalFolders.Add(newFolder);
                PopulateDirectoriesFiles(newFolder);
                PopulateFolder(newFolder);
            }
        }

        private void PopulateDirectoriesFiles(Folder folder)
        {
            string[] files = Directory.GetFiles(folder.directoryFullPath)
                .Where(file => file.EndsWith(".meta") == false)
                .ToArray();
            
            for (int i = 0; i < files.Length; i++)
            {
                files[i] = Path.GetFileNameWithoutExtension(files[i]);
            }
            
            folder.files = files;
        }
    }
}