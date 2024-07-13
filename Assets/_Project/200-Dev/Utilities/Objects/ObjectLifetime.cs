using System.Linq;
using Eflatun.SceneReference;
using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace _Project._200_Dev.Utilities.Objects
{
    public class ObjectLifetime : MonoBehaviour
    {
        [SerializeField] private bool _infiniteLifeTime = true;
        [SerializeField, HideIf(nameof(_infiniteLifeTime)), ListDrawerSettings] private SceneReference[] _authorizedScenes;


        private void Awake()
        {
            if (CanSpawn())
            {
                ObjectsLifetimeManager.instance.Register(this);
            }
            else Destroy();
        }

        private void OnDestroy()
        {
            // In case we destroy it in Awake, the object is not registered.
            if (didStart) ObjectsLifetimeManager.instance.Unregister(this);
        }


        public bool CanSpawn()
        {
            if (_infiniteLifeTime) return true;
            
            for (int i = 0; i < SceneManager.sceneCount; i++)
            {
                Scene scene = SceneManager.GetSceneAt(i);

                if (_authorizedScenes.Any(t => scene.buildIndex == t.BuildIndex)) continue;
                
                return false;
            }
            
            return true;
        }
        
        public void SetToDontDestroyOnLoad()
        {
            DontDestroyOnLoad(gameObject);
        }
        
        public void OnSceneLoaded(Scene scene)
        {
            if (_infiniteLifeTime) return;
            
            for (int i = 0; i < _authorizedScenes.Length; i++)
            {
                if (_authorizedScenes[i].BuildIndex != scene.buildIndex) Destroy();
            }
        }

        private void Destroy()
        {
            Destroy(gameObject);
            Debug.Log($"[ObjectLifetime] {name} is being destroyed cause he entered an unauthorized scene.\n" +
                      "Authorized scenes are: " + string.Join(", ", _authorizedScenes.Select(t => t.Name)));
        }
    }
}