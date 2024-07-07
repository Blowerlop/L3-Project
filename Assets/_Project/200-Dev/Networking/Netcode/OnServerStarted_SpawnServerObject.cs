using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace Project
{
    public class OnServerStarted_SpawnServerObject : MonoBehaviour
    {
        [SerializeField, AssetsOnly] private NetworkObject _serverObject;
        
        private void Start()
        {
            NetworkManager.Singleton.OnServerStarted += SpawnServerObject;
        }

        private void OnDestroy()
        {
            if (NetworkManager.Singleton == null) return;
            
            NetworkManager.Singleton.OnServerStarted -= SpawnServerObject;
        }

        private void SpawnServerObject()
        {
            if (NetworkManager.Singleton.IsServer == false) return;

            if (_serverObject != null)
            {
                NetworkObject serverObjectInstance = Instantiate(_serverObject);
                serverObjectInstance.Spawn(false);
            }
        }
    }
}
