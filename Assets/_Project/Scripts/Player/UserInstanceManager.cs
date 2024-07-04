using System.Collections.Generic;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;

namespace Project
{
    public class UserInstanceManager : NetworkSingleton<UserInstanceManager>
    {
        [SerializeField, Required, AssetsOnly] private UserInstance _userInstancePrefab;
        [ShowInInspector, ReadOnly] private readonly Dictionary<int, UserInstance> _userInstances = new Dictionary<int, UserInstance>();
        public int count => _userInstances.Count;


        public override void OnNetworkSpawn()
        {
            if (!IsServer && !IsHost) return;

            base.OnNetworkSpawn();
            NetworkManager.Singleton.OnClientConnectedCallback += CreateNetcodeUserInstance;
            NetworkManager.Singleton.OnClientDisconnectCallback += DestroyNetcodeUserInstance;
        }
        
        public override void OnNetworkDespawn()
        {
            if (!IsServer && !IsHost) return;
            
            base.OnNetworkDespawn();
            NetworkManager.Singleton.OnClientConnectedCallback -= CreateNetcodeUserInstance;
            NetworkManager.Singleton.OnClientDisconnectCallback -= DestroyNetcodeUserInstance;
        }

        
        private void CreateNetcodeUserInstance(ulong clientId)
        {
            if (_userInstances.ContainsKey((int)clientId))
            {
                Debug.LogError($"The client {clientId} has already a userInstance registered");
                return;
            }

            // Spawn UserInstance 
            Debug.Log("[UserInstanceManager] Create Unity UserInstance");
            
            UserInstance userInstance = Instantiate(_userInstancePrefab); 
            userInstance.SrvSetClientId((int)clientId);
            
            _userInstances.Add((int)clientId, userInstance);
            
            userInstance.GetComponent<NetworkObject>().SpawnWithOwnership(clientId);
        }
        
        private void DestroyNetcodeUserInstance(ulong clientId)
        {
            if (_userInstances.ContainsKey((int)clientId) == false)
            {
                Debug.LogError($"The client {clientId} has no userInstance registered");
                return;
            }
            
            Debug.Log("[UserInstanceManager] Destroy Unity UserInstance");
        
            UserInstance userInstance = _userInstances[(int)clientId];
            _userInstances.Remove((int)clientId);
            
            userInstance.GetComponent<NetworkObject>().Despawn(true);
        }
 
        public void ClientRegisterUserInstance(UserInstance inst)
        {
            if (!IsClient || IsHost) return;
            
            if(_userInstances.ContainsKey(inst.ClientId))
            {
                Debug.LogError($"UserInstance already registered for client {inst.ClientId}");
                return;
            }
            
            _userInstances.Add(inst.ClientId, inst);
        }
        
        public void ClientUnregisterUserInstance(UserInstance inst)
        {
            if (!IsClient || IsHost) return;
            
            if(!_userInstances.ContainsKey(inst.ClientId))
            {
                Debug.LogError($"UserInstance is not registered for client {inst.ClientId}");
                return;
            }
            
            _userInstances.Remove(inst.ClientId);
        }
        
        public UserInstance[] GetUsersInstance()
        {
            UserInstance[] usersInstance = new UserInstance[_userInstances.Count];

            int counter = 0;
            foreach (var userInstance in _userInstances.Values)
            {
                usersInstance[counter] = userInstance;
                counter++;
            }

            return usersInstance;
        }

        public bool TryGetUserInstance(int clientId, out UserInstance userInstance)
        {
            userInstance = GetUserInstance(clientId);
            return userInstance != null;
        }
        
        public UserInstance GetUserInstance(int clientId)
        {
            if (_userInstances.TryGetValue(clientId, out UserInstance userInstance))
            {
                return userInstance;
            }
            
            return null;
        }
        
        public bool TryGetUserInstance(string clientName, out UserInstance userInstance)
        {
            userInstance = GetUserInstance(clientName);
            return userInstance != null;
        }
        
        public UserInstance GetUserInstance(string clientName)
        {
            foreach (UserInstance userInstance in _userInstances.Values)
            {
                if (userInstance.name == clientName) return userInstance;
            }

            return null;
        }

        public Dictionary<int, UserInstance>.ValueCollection All()
        {
            return _userInstances.Values;
        }
    }
}
