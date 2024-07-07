using UnityEngine;

namespace _Project._200_Dev.Utilities.Monobehaviour
{
    public class MonoSingleton<T> : MonoBehaviour where T : MonoBehaviour
    {
        protected static bool dontDestroyOnLoad = true;
        
        private static T _instance = null;
        public static T instance {
            get
            {
                if (Application.isPlaying == false) return null;
                
                if(_instance == null) _instance = FindAnyObjectByType<T>();
                
                return _instance;
            }
        }
        
        protected virtual void Awake(){
            if (_instance != null && _instance != this)
            {
                Debug.LogError($"There is more than one instance of {this}");
                Destroy(this);
                return;
            }
            
            _instance = GetComponent<T>();
            
            if(dontDestroyOnLoad)
            {
                DontDestroyOnLoad(gameObject);
            }
        }

        protected virtual void OnDestroy()
        {
            if (_instance == this) _instance = null;
        }

        public static bool IsInstanceAlive() => _instance != null;

        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void ResetStaticVariables()
        {
            _instance = null;
        }
    }
}
