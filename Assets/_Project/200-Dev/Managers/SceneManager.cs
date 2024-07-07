using Unity.Netcode;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace Project
{
    public class SceneManager : NetworkBehaviour
    {
        [ClearOnReload] private static LoadingScreenParameters _currentLoadingScreenParameters;


        private void Start()
        {
            UnityEngine.SceneManagement.SceneManager.sceneLoaded += OnLoadCompleteOffline;
        }

        public override void OnNetworkSpawn()
        {
            NetworkManager.Singleton.SceneManager.OnLoad += OnLoadNetwork;
            NetworkManager.Singleton.SceneManager.OnLoadComplete += OnLoadCompleteNetwork;
        }
        
        public override void OnNetworkDespawn()
        {
            NetworkManager.Singleton.SceneManager.OnLoad -= OnLoadNetwork;
            NetworkManager.Singleton.SceneManager.OnLoadComplete -= OnLoadCompleteNetwork;
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
            
            UnityEngine.SceneManagement.SceneManager.sceneLoaded -= OnLoadCompleteOffline;
        }


        private void OnLoadNetwork(ulong clientId, string sceneName, LoadSceneMode loadSceneMode, AsyncOperation asyncOperation)
        {
            if (IsClient)
            {
                // AsyncOperation is commented because we don't need the loadingBar for the moment
                LoadingScreenManager.Show(_currentLoadingScreenParameters/*, asyncOperation*/);
            }
        }
        
        private void OnLoadCompleteNetwork(ulong clientId, string sceneName, LoadSceneMode loadSceneMode)
        {
            if (IsClient)
            {
                UpdateUserInstanceSceneServerRpc(clientId, sceneName);
                LoadingScreenManager.Hide();  
            }
        }
        
        private void OnLoadCompleteOffline(Scene scene, LoadSceneMode loadSceneMode)
        {
            if (NetworkManager.Singleton != null && NetworkManager.Singleton.IsListening) return;
            
            LoadingScreenManager.Hide();
        }

        [ServerRpc(RequireOwnership = false)]
        private void UpdateUserInstanceSceneServerRpc(ulong clientId, string sceneName)
        {
            if (UserInstanceManager.instance.TryGetUserInstance((int)clientId, out UserInstance userInstance))
            {
                userInstance.SrvSetScene(sceneName);
            }
            else
            {
                Debug.LogError("[SceneManager/OnLoadComplete] UserInstance is null. Ignore if is the first connection");
            }
        }

        [ConsoleCommand("scene_load", "Load a scene")]
        private static void LoadScene(string sceneName, LoadSceneMode loadSceneMode = LoadSceneMode.Single)
        {
            if (NetworkManager.Singleton != null && NetworkManager.Singleton.IsListening)
            {
                Network_LoadSceneAsync(sceneName, loadSceneMode, new LoadingScreenParameters());
            }
            else
            {
                LoadSceneAsync(sceneName, loadSceneMode);
            }
        }

        [Server]
        public static void Network_LoadSceneAsync(string sceneName, LoadSceneMode loadSceneMode, LoadingScreenParameters loadingScreenParameters)
        {
            _currentLoadingScreenParameters = loadingScreenParameters;
            NetworkManager.Singleton.SceneManager.LoadScene(sceneName, loadSceneMode);
        }
        
        public static void LoadSceneAsync(string sceneName, LoadSceneMode loadSceneMode)
        {
            AsyncOperation asyncOperation = UnityEngine.SceneManagement.SceneManager.LoadSceneAsync(sceneName, loadSceneMode);
            LoadingScreenManager.Show(new LoadingScreenParameters()/*, asyncOperation*/);
        }
    }
}