using System.Collections.Generic;
using System.Linq;
using _Project._200_Dev.User;
using TMPro;
using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.Lobby
{
    public class TeamSelectionUI : NetworkBehaviour
    {
        [SerializeField, Min(-1), Tooltip("-1 mean that the team has not been set")] private int _teamIndex = -1;
        [SerializeField] private TMP_Text _pcButtonText;
        [SerializeField] private TMP_Text _mobileButtonText;
        
        #if UNITY_EDITOR
        // Used for debug purpose
        // It permit to advertise if two teams have the same index
        [ClearOnReload(assignNewTypeInstance: true)] private static readonly List<int> _commonTeamIndex = new List<int>((int)TeamManager.MAX_TEAM);
        #endif


        private void Awake()
        {
            UserInstance.OnSpawned += OnClientStarted_UpdateUi;
            UserInstance.OnDespawned += OnClientStarted_UpdateUi;
            UserInstance.OnNameChanged += OnClientStarted_UpdateUi;
        }

        private void Start()
        {
            CheckTeamIndexValidity();
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
            
            UserInstance.OnSpawned -= OnClientStarted_UpdateUi;
            UserInstance.OnDespawned -= OnClientStarted_UpdateUi;
            UserInstance.OnNameChanged -= OnClientStarted_UpdateUi;
        }

        public override void OnNetworkSpawn()
        {
            
            TeamManager.instance.onTeamSet += OnTeamSet_UpdateButtonText;

            // Because right now in testing this network spawn might fired first, we have a null ref on our UserInstance.
            // But in the final game, the UserInstance will be the first thing ever fired in the network (normally)
            if (NetworkManager.IsServer && NetworkManager.IsHost == false) return;
            Utilities.Utilities.StartWaitUntilAndDoAction(this, () => UserInstance.Me != null,
                () =>
                {
                    if (UserInstance.Me == null)
                    {
                        Debug.LogError("UserInstance is null");
                        return;
                    }
                    
                    UserInstance.Me._networkIsReady.OnValueChanged += OnPlayerReady_UpdateButtonTextColor;
                });
            
             // UserInstance.Me._networkIsReady.OnValueChanged += OnPlayerReady_UpdateButtonTextColor;
        }

        public override void OnNetworkDespawn()
        {
            if (TeamManager.IsInstanceAlive())
            {
                TeamManager.instance.onTeamSet -= OnTeamSet_UpdateButtonText;
            }

            if (UserInstance.Me != null)
            {
                UserInstance.Me._networkIsReady.OnValueChanged -= OnPlayerReady_UpdateButtonTextColor;
            }   
        }
        
        private void OnValidate()
        {
            _pcButtonText.text = TeamManager.DEFAULT_PC_SLOT_TEXT;
            _mobileButtonText.text = TeamManager.DEFAULT_MOBILE_SLOT_TEXT;
        }

        private void CheckTeamIndexValidity()
        {
            if (TeamManager.instance.IsTeamIndexValid(_teamIndex) == false)
            {
                Debug.LogError("Team index is invalid : " + _teamIndex);
            }

            #if UNITY_EDITOR
            _commonTeamIndex.Add(_teamIndex);
            int countSameOccurence = _commonTeamIndex.Count(x => x == _teamIndex);
            if (countSameOccurence > 1)
            {
                Debug.LogError($"{countSameOccurence} teams have the team index {_teamIndex}");
            }
            #endif
        }
        
        public void SetTeamServerRpcc()
        {
            SetTeamServerRpc(_teamIndex, NetworkManager.Singleton.LocalClientId);
        }

        [ServerRpc(RequireOwnership = false)]
        private void SetTeamServerRpc(int teamIndex, ulong ownerClientId)
        {
            UserInstance userInstance = UserInstanceManager.instance.GetUserInstance((int)ownerClientId); 
            if (userInstance == null)
            {
                Debug.LogError("User instance is null");
                return;
            }

            TeamManager.instance.TrySetTeam((int)ownerClientId, teamIndex);
        }

        private void OnTeamSet_UpdateButtonText(int teamIndex, string clientName)
        {
            if (teamIndex != _teamIndex) return;

            UpdatePcButtonTextClientRpc(clientName);
        }
        
        [ClientRpc]
        private void UpdatePcButtonTextClientRpc(string clientName)
        {
            UpdatePcButtonTextLocal(clientName);
        }
        
        [ClientRpc]
        private void UpdatePcButtonTextClientRpc(string clientName, ClientRpcParams clientRpcParams)
        {
            UpdatePcButtonTextLocal(clientName);
        }

        private void UpdatePcButtonTextLocal(string clientName)
        {
            _pcButtonText.text = clientName == TeamManager.DEFAULT_PC_SLOT_TEXT ? clientName : $"<color=#C89B3C>{clientName}</color>";
        }

        private void OnPlayerReady_UpdateButtonTextColor(bool _, bool readyState)
        {
            OnPlayerReady_UpdateButtonTextColorServerRpc((int)NetworkManager.Singleton.LocalClientId, readyState);
        }
        
        [ServerRpc(RequireOwnership = false)]
        private void OnPlayerReady_UpdateButtonTextColorServerRpc(int clientId, bool readyState)
        {
            if (_teamIndex == TeamManager.UNASSIGNED_TEAM_INDEX) return;
            
            TeamData teamData = TeamManager.instance.GetTeamData(_teamIndex);

            UpdatePcButtonTextColorClientRpc(_teamIndex, readyState);
        }

        [ClientRpc]
        private void UpdatePcButtonTextColorClientRpc(int teamIndex, bool state)
        {
            UpdatePcButtonTextColorLocal(teamIndex, state);
        }
        
        [ClientRpc]
        private void UpdatePcButtonTextColorClientRpc(int teamIndex, bool state, ClientRpcParams clientRpcParams)
        {
            UpdatePcButtonTextColorLocal(teamIndex, state);
        }

        private void UpdatePcButtonTextColorLocal(int teamIndex, bool state)
        {
            if (teamIndex != _teamIndex) return;
         
            _pcButtonText.color = state ? Color.green : Color.black;
            // Player name use rich text, so we need to remove it in order to see the normal text color
            if (state) _pcButtonText.text = _pcButtonText.GetParsedText();
        }
        
        private void OnClientStarted_UpdateUi(UserInstance userInstance)
        {
            ulong clientId = userInstance.OwnerClientId;
            
            if (!IsServer) return;
            
            ClientRpcParams sendParams = new ClientRpcParams
            {
                Send = new ClientRpcSendParams
                {
                    TargetClientIds = new[] { clientId }
                }
            };

            if (_teamIndex == TeamManager.UNASSIGNED_TEAM_INDEX)
            {
                var unassignedUserInstances = UserInstanceManager.instance.All().Where(x => x.Team == TeamManager.UNASSIGNED_TEAM_INDEX).ToArray();
                
                string pcPlayersName = string.Join(" / ", unassignedUserInstances.Select(x => x.PlayerName));
                if (string.IsNullOrEmpty(pcPlayersName)) pcPlayersName = TeamManager.DEFAULT_PC_SLOT_TEXT;
                
                UpdatePcButtonTextClientRpc(pcPlayersName);
                
                return;
            }
            
            TeamData teamData = TeamManager.instance.GetTeamData(_teamIndex);
            if (teamData.TryGetUserInstance(out userInstance))
            {
                UpdatePcButtonTextClientRpc(userInstance.PlayerName, sendParams);
                if (userInstance.IsReady) UpdatePcButtonTextColorClientRpc(_teamIndex, true, sendParams);
            }
            else UpdatePcButtonTextClientRpc(TeamManager.DEFAULT_PC_SLOT_TEXT, sendParams);
        }
    }
}
