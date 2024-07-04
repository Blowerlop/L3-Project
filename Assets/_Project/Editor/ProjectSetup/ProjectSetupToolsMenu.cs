using System.IO;
using UnityEditor;
using UnityEngine;
using static System.IO.Directory;

namespace _Project.Editor
{
    public static class ProjectSetupToolsMenu
    {
        [MenuItem("Tools/Setup/Create Default Folders")]
        public static void CreateDefaultFolders()
        {
            Debug.Log("Setting up the default folders...");

            CreateDirectories("_Project", "Arts", "Resources", "Scripts", "Prefabs", "Data", "Animations", "VFX", "Scenes");
            CreateDirectories("_Project/Arts", "Sprites", "Materials", "Textures", "Shaders");
            CreateDirectories("_Project/Scripts", "Managers");

            AssetDatabase.Refresh();
        
            Debug.Log("Done !");
        }

        private static void CreateDirectories(string root, params string[] directoriesName)
        {
            string fullPath = Path.Combine(Application.dataPath, root);

            foreach (var newDirectory in directoriesName)
            {
                CreateDirectory(Path.Combine(fullPath, newDirectory));
            }
        }
    }
}