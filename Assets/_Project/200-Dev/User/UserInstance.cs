using System;
using _Project._200_Dev.Entities.Player;
using _Project._200_Dev.Lobby;
using _Project._200_Dev.Menu;
using _Project._200_Dev.Networking.IL_Weaving.Server;
using Sirenix.OdinInspector;
using Unity.Collections;
using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.User
{
    /// <summary>
    /// Stores client's specific infos.
    /// </summary>
    public class UserInstance : NetworkBehaviour
    {
        /// <summary>
        /// Reference to the local client's UserInstance.
        /// </summary>
        [ClearOnReload] public static UserInstance Me;
        public static event Action<UserInstance> OnSpawned;
        public static event Action<UserInstance> OnLocalSpawned;
        public static event Action<UserInstance> OnDespawned;
        public static event Action<UserInstance> OnLocalDespawned;
        public static event Action<UserInstance> OnNameChanged;

        public static event Action OnTeamChangedEvent;

        public PlayerRefs LinkedPlayer { get; private set; }
        public event Action<PlayerRefs> OnPlayerLinked;
        
        //NetVars
        [ShowInInspector] private NetworkVariable<int> _networkClientId = new(int.MaxValue);
        [ShowInInspector] private NetworkVariable<FixedString64Bytes> _networkScene = new("Scene");
        [ShowInInspector] private NetworkVariable<FixedString64Bytes> _networkPlayerName = new("UnknowName");
        [ShowInInspector] public NetworkVariable<int> _networkTeam = new(-1);
        [ShowInInspector] private NetworkVariable<bool> _networkIsMobile = new();
        [ShowInInspector] public NetworkVariable<bool> _networkIsReady { get; private set; } = new();
        [ShowInInspector] private NetworkVariable<int> _networkCharacterId = new();

        [ShowInInspector] public NetworkVariable<int> WinCount { get; private set; } = new();
        
        private NetworkVariable<int> _ms1 = new(), _ms2 = new(), _ms3 = new(), _ms4 = new();
        
        public int ClientId => _networkClientId.Value;
        
        public string PlayerName => _networkPlayerName.Value.ToString();
        public int Team => _networkTeam.Value;
        public bool IsReady => _networkIsReady.Value;
        public int CharacterId => _networkCharacterId.Value;

        private void Start()
        {
            _networkPlayerName.OnValueChanged += OnPlayerNameChanged_NotifyAll;
        }

        

        public override void OnDestroy()
        {
            base.OnDestroy();
            
            _networkPlayerName.OnValueChanged -= OnPlayerNameChanged_NotifyAll;
        }

        public override void OnNetworkSpawn()
        {
            Debug.Log("[UserInstance] Start spawn", gameObject);

            if (IsClient && !IsHost)
            {
                // OnValueChanged is not called for network object that were already spawned before joining
                // We need to call manually
                if(_networkClientId.Value != int.MaxValue) OnClientIdChanged(0, _networkClientId.Value);
                if(_networkTeam.Value != -1) OnTeamChanged(-1, _networkTeam.Value);
                
                _networkClientId.OnValueChanged += OnClientIdChanged;
                _networkTeam.OnValueChanged += OnTeamChanged;
            }

            if (IsServer)
            {
                string currentSceneName = UnityEngine.SceneManagement.SceneManager.GetActiveScene().name;
                SrvSetScene(currentSceneName);
            }
            
            if (IsOwner)
            {
                Me = this;
                SetNameServerRpc(PlayerData.playerName);  
                OnLocalSpawned?.Invoke(this);
            }

            OnSpawned?.Invoke(this);
            Debug.Log("[UserInstance] End spawn", gameObject);
        }
        
        public override void OnNetworkDespawn()
        {
            Debug.Log("[UserInstance] Start despawn", gameObject);

            if (IsClient)
            {
                if(UserInstanceManager.instance) UserInstanceManager.instance.ClientUnregisterUserInstance(this);
                _networkClientId.OnValueChanged -= OnClientIdChanged;
                _networkTeam.OnValueChanged -= OnTeamChanged;
            }
            
            if (IsOwner)
            {
                Me = null;
                OnLocalDespawned?.Invoke(this);
            }
            
            OnDespawned?.Invoke(this);
            Debug.Log("[UserInstance] End despawn", gameObject);
        }

        public void LinkPlayer(PlayerRefs refs)
        {
            Debug.Log($"LinkPlayer for UserInstance {_networkClientId.Value}");
            LinkedPlayer = refs;
            
            OnPlayerLinked?.Invoke(refs);
        }

        public void UnlinkPlayer()
        {
            Debug.Log($"UnlinkPlayer for UserInstance {_networkClientId.Value}");
            LinkedPlayer = null;
            
            // Really useful ?
            OnPlayerLinked?.Invoke(null);
        }
        
        private void OnClientIdChanged(int oldValue, int newValue)
        {
            // Should only happen once when user instance is spawned
            
            Utilities.Utilities.StartWaitUntilAndDoAction(this, UserInstanceManager.IsInstanceAlive, () =>
            {
                UserInstanceManager.instance.ClientRegisterUserInstance(this);
            });
        }
        
        private void OnTeamChanged(int oldValue, int newValue)
        {            
            Utilities.Utilities.StartWaitUntilAndDoAction(this, TeamManager.IsInstanceAlive, () =>
            {
                TeamManager.instance.ClientOnTeamChanged(this, oldValue, newValue);
                OnTeamChangedEvent?.Invoke();
            });
        }
        
        //Setters
        [Server]
        [Button]
        public void SrvSetClientId(int clientId)
        {
            _networkClientId.Value = clientId;
        }
        
        [Server]
        [Button]
        public void SrvSetScene(string sceneName)
        {
            _networkScene.Value = sceneName;
        }
        
        [Server]
        [Button]
        public void SrvSetName(string playerName)
        {
            _networkPlayerName.Value = playerName;
        }
        
        [ServerRpc]
        public void SetNameServerRpc(string playerName)
        {
            SrvSetName(playerName);
        }
        
        [Server]
        [Button]
        public void SrvSetTeam(int playerTeam)
        {
            _networkTeam.Value = playerTeam;
            OnTeamChangedEvent?.Invoke();
        }

        [Server]
        [Button]
        public void SrvSetIsMobile(bool isMobile)
        {
            _networkIsMobile.Value = isMobile;
        }
        
        [Server]
        [Button]
        public void SrvSetIsReady(bool isReady)
        {
            _networkIsReady.Value = isReady;
        }

        [Server]
        [Button]
        public void SrvSetCharacter(int characterId)
        {
            _networkCharacterId.Value = characterId;
        }

        
        private void OnPlayerNameChanged_NotifyAll(FixedString64Bytes previousvalue, FixedString64Bytes newvalue)
        {
            OnNameChanged?.Invoke(this);
        }
        
        #if UNITY_EDITOR
        [ExecuteOnReload]
        private static void StaticClear()
        {
            OnSpawned = null;
        }
        #endif
    }
}