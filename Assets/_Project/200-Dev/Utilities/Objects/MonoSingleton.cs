using UnityEngine;

namespace _Project._200_Dev.Utilities.Objects
{
    public class MonoSingleton<T> : MonoBehaviour where T : MonoBehaviour
    {
        private static T _instance;
        public static T instance {
            get
            {
                #if UNITY_EDITOR
                if (Application.isPlaying == false) return null;
                #endif
                
                if(_instance == null) _instance = FindAnyObjectByType<T>();
                
                return _instance;
            }
        }
        
        protected virtual void Awake()
        {
            if (_instance != null && _instance != this)
            {
                Debug.LogError($"[{nameof(MonoSingleton<T>)}] There is more than one instance of {this}");
                Destroy(this);
                return;
            }

            // We may have found the object in the instance getter.
            if (_instance == null) _instance = GetComponent<T>();
        }

        #if UNITY_EDITOR
        protected virtual void OnDestroy()
        {
            if (_instance == this) _instance = null;
        }

        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void ResetStaticVariables()
        {
            _instance = null;
        }
        #endif
    }
}
