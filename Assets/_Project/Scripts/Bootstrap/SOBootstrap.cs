using System;
using Sirenix.OdinInspector;
using UnityEditor;
using UnityEngine;
using Object = UnityEngine.Object;

namespace Project
{
    [CreateAssetMenu(menuName = "Scriptable Objects/Boostrap")]
    public class SOBootstrap : ScriptableObject
    {
        [field: Title("Subsystem Registration")]
        [field: Tooltip("Callback invoked when starting up the runtime. Called before the first scene is loaded.")]
        [field: SerializeField] public Object[] subsystemRegistration { get; private set; }
        
        [field: Title("After Assemblies Loaded")]
        [field: Tooltip("Callback invoked when all assemblies are loaded and preloaded assets are initialized. At this time the objects of the first scene have not been loaded yet.")]
        [field: SerializeField] public Object[] afterAssembliesLoaded { get; private set; }
        
        [field: Title("Before Splash Screen")]
        [field: Tooltip("Callback invoked before the splash screen is shown. At this time the objects of the first scene have not been loaded yet.")]
        [field: SerializeField] public Object[] beforeSplashScreen { get; private set; }
        
        [field: Title("Before Scene Load")]
        [field: Tooltip("Callback invoked when the first scene's objects are loaded into memory but before Awake has been called.")]
        [field: SerializeField] public Object[] beforeSceneLoad { get; private set; }
        
        [field: Title("After Scene Load")] 
        [field: Tooltip("Callback invoked when the first scene's objects are loaded into memory and after Awake has been called.")]
        [field: SerializeField] public Object[] afterSceneLoad { get; private set; }

#if UNITY_EDITOR
        public Object[] GetObjects(RuntimeInitializeLoadType runtimeInitializeLoadType)
        {
            return runtimeInitializeLoadType switch
            {
                RuntimeInitializeLoadType.AfterSceneLoad => afterSceneLoad,
                RuntimeInitializeLoadType.BeforeSceneLoad => beforeSceneLoad,
                RuntimeInitializeLoadType.AfterAssembliesLoaded => afterAssembliesLoaded,
                RuntimeInitializeLoadType.BeforeSplashScreen => beforeSplashScreen,
                RuntimeInitializeLoadType.SubsystemRegistration => subsystemRegistration,
                _ => throw new ArgumentOutOfRangeException(nameof(runtimeInitializeLoadType), runtimeInitializeLoadType,
                    null)
            };
        }
        
        [ParrelSyncIgnore]
        public void SetObjects(RuntimeInitializeLoadType runtimeInitializeLoadType, Object[] obj)
        {
            switch (runtimeInitializeLoadType)
            {
                case RuntimeInitializeLoadType.AfterSceneLoad:
                    afterSceneLoad = obj;
                    break;
                
                case RuntimeInitializeLoadType.BeforeSceneLoad:
                    beforeSceneLoad = obj;
                    break;
                
                case RuntimeInitializeLoadType.AfterAssembliesLoaded:
                    afterAssembliesLoaded = obj;
                    break;
                
                case RuntimeInitializeLoadType.BeforeSplashScreen:
                    beforeSplashScreen = obj;
                    break;
                
                case RuntimeInitializeLoadType.SubsystemRegistration:
                    subsystemRegistration = obj;
                    break;
                
                default:
                    throw new ArgumentOutOfRangeException(nameof(runtimeInitializeLoadType), runtimeInitializeLoadType, null);
            }
            
            ForceSaveOnDisk();
        }
        
        [Button]
        [Tooltip("Sometimes Github don't detect the changes made on the ScriptableObject, so we need to force the write on the disk")]
        [ParrelSyncIgnore]
        private void ForceSaveOnDisk()
        {
            EditorUtility.SetDirty(this);
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
        }
        #endif
    }
}
