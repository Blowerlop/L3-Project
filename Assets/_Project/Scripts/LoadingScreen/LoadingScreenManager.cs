using System.Collections;
using Project.Extensions;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.Events;

namespace Project
{
    public struct LoadingScreenParameters
    {
        public readonly Sprite backgroundSprite;
        public readonly Color? backgroundColor;

        public LoadingScreenParameters(Sprite backgroundSprite, Color? backgroundColor)
        {
            this.backgroundSprite = backgroundSprite;
            this.backgroundColor = backgroundColor;
        }
    }
    
    public class LoadingScreenManager : MonoBehaviour
    {
        [SerializeField, RequiredIn(PrefabKind.PrefabAsset), AssetsOnly] private LoadingScreen _loadingScreenPrefab;
        [ClearOnReload] private static LoadingScreenManager _instance;
        [ClearOnReload] private static LoadingScreen _loadingScreenInstance;
        [ClearOnReload] private static Coroutine _showCoroutine;

        [SerializeField] private UnityEvent _onShowEvent = new UnityEvent();
        [SerializeField] private UnityEvent _onHideEvent = new UnityEvent();
        


        private void Awake()
        {
            // Lazy singleton
            _instance = this;
        }

        [Button]
        public static void Show(LoadingScreenParameters loadingScreenParameters, AsyncOperation asyncOperation = null)
        {
            ShowBehaviour(loadingScreenParameters, asyncOperation);
        }
        
        private static void ShowBehaviour(LoadingScreenParameters loadingScreenParameters, AsyncOperation asyncOperation = null)
        {
            if (_loadingScreenInstance != null) return;

            _showCoroutine = _instance.StartCoroutine(ShowBehaviourCoroutine(loadingScreenParameters, asyncOperation));
        }

        private static IEnumerator ShowBehaviourCoroutine(LoadingScreenParameters loadingScreenParameters, AsyncOperation asyncOperation = null)
        {
            Debug.Log("LoadingScreen Show");
            
            _loadingScreenInstance = Instantiate(_instance._loadingScreenPrefab);
            DontDestroyOnLoad(_loadingScreenInstance);
            _loadingScreenInstance.UpdateLoadingScreen(loadingScreenParameters);
            _instance._onShowEvent.Invoke();

            LoadingBar loadingBar = _loadingScreenInstance.transform.GetComponentInChildren<LoadingBar>();
            
            if (asyncOperation != null)
            {
                asyncOperation.allowSceneActivation = NetworkManager.Singleton.IsListening;
                
                loadingBar.IsNull()?.SetActive(true);
                
                while (asyncOperation.progress <= 0.89f)
                {
                    loadingBar.IsNull()?.UpdateLoadingBar((asyncOperation.progress / 0.9f) * 100);
                    yield return null; 
                }

                yield return new WaitForSecondsRealtime(0.1f);
                asyncOperation.allowSceneActivation = true;
            }
            else
            {
                loadingBar.IsNull()?.SetActive(false);
            }
        }
        
        [Button]
        public static void Hide()
        {
            if (IsAlive() == false) return;
            Debug.Log("LoadingScreen Hide");
            _instance._onHideEvent.Invoke();

            Destroy(_loadingScreenInstance.gameObject);
            _loadingScreenInstance = null;

            
            if (_showCoroutine == null) return;
            
            _instance.StopCoroutine(_showCoroutine);
            _showCoroutine = null;
        }

        public static bool IsAlive() => _loadingScreenInstance != null;
    }
}
