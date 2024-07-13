using System.Collections.Generic;
using UnityEngine.SceneManagement;

namespace _Project._200_Dev.Utilities.Objects
{
    public class ObjectsLifetimeManager : MonoSingleton<ObjectsLifetimeManager>
    {
        private readonly List<ObjectLifetime> _objectsLifetime = new(10);
        
        
        private void OnEnable()
        {
            SceneManager.sceneLoaded += OnSceneLoaded_ManageObjectsLifetime;
        }

        private void OnDisable()
        {
            SceneManager.sceneLoaded -= OnSceneLoaded_ManageObjectsLifetime;
        }
        
        
        private void OnSceneLoaded_ManageObjectsLifetime(Scene scene, LoadSceneMode _)
        {
            for (int i = 0; i < _objectsLifetime.Count; i++)
            {
                _objectsLifetime[i].OnSceneLoaded(scene);
            }
        }
        
        public void Register(ObjectLifetime objectLifetime)
        {
            if (objectLifetime.CanSpawn())
            {
                objectLifetime.SetToDontDestroyOnLoad();
                _objectsLifetime.Add(objectLifetime);
            }
        }
        
        public void Unregister(ObjectLifetime objectLifetime)
        {
            _objectsLifetime.Remove(objectLifetime);
        }
    }
}