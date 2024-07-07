using System;
using System.Collections.Generic;
using System.Linq;
using Sirenix.OdinInspector;
using Sirenix.Utilities;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
using UnityEditor.Build;
using UnityEditor.Build.Reporting;
#endif

//
// Inspired of https://gist.github.com/Lazersquid/4f04327da0741f2e1dea5038026701f2
//
namespace Project
{
    [CreateAssetMenu(menuName = "Scriptable Objects/Reference Cache", fileName = "SO References Cache")]
    public class SOScriptableObjectReferencesCache : ScriptableObject
    {
#if UNITY_EDITOR
        [field: Title("Configuration")]
        [field: SerializeField] public bool autoFetchInPlaymode { get; private set; } = true;
#endif

        private static SOScriptableObjectReferencesCache _instance;
        public static SOScriptableObjectReferencesCache instance
        {
            get
            {
                if (_instance == null)
                {
                    _instance = Resources.Load<SOScriptableObjectReferencesCache>("SO References Cache");
                }

                return _instance;
            }
        }
        
        [Title("Data")]
        // I can't make it dictionary because Unity can't serialize them 
        [field: SerializeField, ReadOnly] private List<SOCacheEntry> _scriptableObjectsCache;

        public static T[] GetScriptableObjects<T>()
        {
            return instance._scriptableObjectsCache
                .Find(soCacheEntry => soCacheEntry.typeName == typeof(T).Name)?
                .scriptableObjects
                .Cast<T>()
                .ToArray();
        }
        
        public static ScriptableObject[] GetScriptableObjects(Type type)
        {
            return instance._scriptableObjectsCache.Find(soCacheEntry => soCacheEntry.typeName == type.Name)
                .scriptableObjects;
        }
        
        #if UNITY_EDITOR
        [ButtonGroup]
        public static void FetchReferences()
        {
            ClearReferences();

            instance._scriptableObjectsCache = new List<SOCacheEntry>();
            
            var types = GetSoTypesWithInterface<IScriptableObjectSerializeReference>();
            foreach (Type type in types)
            {
                ScriptableObject[] scriptableObjects = Utilities.FindAssetsByType<ScriptableObject>(type).Where(t => t.GetType() == type).ToArray();
                if (scriptableObjects.Length == 0) continue;
                instance._scriptableObjectsCache.Add(new SOCacheEntry(type.Name, scriptableObjects));
            }
            
            instance.ForceSaveOnDisk();
        }

        [ButtonGroup]
        private static void ClearReferences()
        {
            instance._scriptableObjectsCache = null;
        }
        
        private static Type[] GetSoTypesWithInterface<T>()
        {
            return AssemblyUtilities.GetTypes(AssemblyTypeFlags.All)
                .Where(t =>
                    !t.IsAbstract &&
                    !t.IsGenericType &&
                    typeof(T).IsAssignableFrom(t) &&
                    t.IsSubclassOf(typeof(ScriptableObject)))
                .ToArray();
        }
        
        [Button]
        [ParrelSyncIgnore]
        private void ForceSaveOnDisk()
        {
            EditorUtility.SetDirty(this);
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
        }
        #endif 
    }
    
    
    
    #if UNITY_EDITOR
    public class ScriptableObjectReferencesCacheEditor : IPreprocessBuildWithReport
    {
        [InitializeOnLoadMethod]
        private static void Subscribe()
        {
            EditorApplication.playModeStateChanged += OnExitingEditMode;
        }

        private static void OnExitingEditMode(PlayModeStateChange state)
        {
            if (state != PlayModeStateChange.ExitingEditMode) return;
            
            if (SOScriptableObjectReferencesCache.instance.autoFetchInPlaymode)
            {
                SOScriptableObjectReferencesCache.FetchReferences();
            }
        }
        
        public int callbackOrder { get; }
        public void OnPreprocessBuild(BuildReport report)
        {
            SOScriptableObjectReferencesCache.FetchReferences();
        }
    }
#endif
    
    [Serializable]
    public class SOCacheEntry
    {
        [field: SerializeField] public string typeName { get; private set; }
        [field: SerializeField] public ScriptableObject[] scriptableObjects { get; private set; }

        
        public SOCacheEntry(string typeName, ScriptableObject[] scriptableObjects)
        {
            this.typeName = typeName;
            this.scriptableObjects = scriptableObjects;
        }
    }
    
    public interface IScriptableObjectSerializeReference
    {
        
    }
}