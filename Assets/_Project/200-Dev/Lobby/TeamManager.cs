using System;
using System.Linq;
using _Project._200_Dev.User;
using _Project._200_Dev.Utilities.NetworkBehaviour;
using Unity.Netcode;
using UnityEngine;

namespace _Project._200_Dev.Lobby
{
    [Serializable]
    public struct TeamData
    {
        public int pcPlayerOwnerClientId;

        public bool HasPC => pcPlayerOwnerClientId != TeamManager.UNASSIGNED_TEAM_INDEX;
        
        public UserInstance GetUserInstance()
        {
            return UserInstanceManager.instance == null ? null : UserInstanceManager.instance.GetUserInstance(pcPlayerOwnerClientId);
        }

        public bool TryGetUserInstance(out UserInstance user)
        {
            user = null;

            if (!UserInstanceManager.instance) return false;

            if (pcPlayerOwnerClientId == TeamManager.UNASSIGNED_TEAM_INDEX) return false;
            var clientId = pcPlayerOwnerClientId;

            return UserInstanceManager.instance.TryGetUserInstance(clientId, out user);
        }
    }
    
    public class TeamManager : NetworkSingleton<TeamManager>
    {
        public const uint MAX_TEAM = 3;
        public const uint MAX_PLAYER = MAX_TEAM * 2;
        public const int UNASSIGNED_TEAM_INDEX = -1;
        // I cant ShowInInspector this array. It weirdly override my value set by the script
        private readonly TeamData[] _teams = new TeamData[MAX_TEAM];

        public const string DEFAULT_PC_SLOT_TEXT = "CLICK TO JOIN";
        public const string DEFAULT_MOBILE_SLOT_TEXT = "<i>Mobile  -  EMPTY</i>"; 


        public Action<int, string> onTeamSet;

        
        public override void OnNetworkSpawn()
        {
            InitializeTeamsData();
            if (IsServer) UserInstance.OnDespawned += OnUserInstanceDespawned_CleanTeam;
        }

        public override void OnNetworkDespawn()
        {
            if (IsServer) UserInstance.OnDespawned -= OnUserInstanceDespawned_CleanTeam;
        }


        private void InitializeTeamsData()
        {
            Debug.Log("InitializeTeamsData");
            for (int i = 0; i < _teams.Length; i++)
            {
                TeamData teamData = new TeamData
                {
                    pcPlayerOwnerClientId = UNASSIGNED_TEAM_INDEX,
                };

                _teams[i] = teamData;
            }
        }


        public bool TryGetTeam(int teamIndex, out TeamData teamData)
        {
            if (IsTeamIndexValid(teamIndex) == false)
            {
                teamData = default;
                return false;
            }
            
            teamData = _teams[teamIndex];
            
            return true;
        }

        public bool TrySetTeam(int ownerClientId, int teamIndex)
        {
            if (IsTeamIndexValid(teamIndex) == false)
            {
                Debug.LogError($"Try to set an invalid team : Invalid is {teamIndex}");
                return false;
            }

            if (IsTeamPlayerSlotAvailable(teamIndex) == false)
            {
                Debug.Log("Trying to join a team slot that are already occupied");
                return false;
            }

            if (UserInstanceManager.instance.GetUserInstance(ownerClientId).IsReady)
            {
                Debug.Log("Trying to join a team while being ready");
                return false;
            }

            Debug.Log("Try set team ok");
            SetTeam(ownerClientId, teamIndex);
            return true;
        }
        
        private void SetTeam(int ownerClientId, int teamIndex)
        {
            UserInstance userInstance = UserInstanceManager.instance.GetUserInstance(ownerClientId);
            
            int previousUserTeamIndex = userInstance.Team;
            // Reset previous team slot if valid
            if (IsTeamIndexValid(previousUserTeamIndex))
            {
                string playerName = string.Empty;
                
                if (previousUserTeamIndex == UNASSIGNED_TEAM_INDEX)
                {
                    playerName = string.Join(" / ", UserInstanceManager.instance.All().Where(x => x.Team == UNASSIGNED_TEAM_INDEX && x.ClientId != ownerClientId).Select(x => x.PlayerName));
                    
                    if (string.IsNullOrEmpty(playerName))
                        playerName = DEFAULT_PC_SLOT_TEXT;
                }
                else
                {
                    ResetTeamSlot(previousUserTeamIndex);
                }
                
                // Always empty if previous team was unassigned
                if (string.IsNullOrEmpty(playerName))
                    playerName = DEFAULT_PC_SLOT_TEXT;
                
                
                OnTeamSet(previousUserTeamIndex, playerName);
            }
            
            if (teamIndex != UNASSIGNED_TEAM_INDEX) RegisterToTeamSlotLocal(ownerClientId, teamIndex);
            userInstance.SrvSetTeam(teamIndex);

            if (teamIndex == UNASSIGNED_TEAM_INDEX)
            {
                string playersName = string.Join(" / ", UserInstanceManager.instance.All().Where(x => x.Team == UNASSIGNED_TEAM_INDEX).Select(x => x.PlayerName));
                
                OnTeamSet(teamIndex, playersName);
            }
            else OnTeamSet(teamIndex, userInstance.PlayerName);
            

            if (teamIndex == UNASSIGNED_TEAM_INDEX)
            {
                Debug.Log("Team recap:\n" +
                          $"Client id {ownerClientId} unassigned from team index {previousUserTeamIndex}");
            }
            else
            {
                Debug.Log("Team recap :\n" +
                          $"Index : {teamIndex}\n" +
                          $"Pc : {_teams[teamIndex].pcPlayerOwnerClientId}\n");
            }
        }

        private void RegisterToTeamSlotLocal(int ownerClientId, int teamIndex)
        {
            TeamData teamData = _teams[teamIndex];
            teamData.pcPlayerOwnerClientId = ownerClientId;
            _teams[teamIndex] = teamData;
        }
        
        /// <summary>
        /// Called by UserInstance._networkTeam callback on clients to populate team array
        /// </summary>
        public void ClientOnTeamChanged(UserInstance user, int oldTeam, int newTeam)
        {
            if (IsTeamIndexValid(oldTeam))
            {
                if (oldTeam != UNASSIGNED_TEAM_INDEX) ResetTeamSlot(oldTeam);
                
                OnTeamSet(oldTeam, DEFAULT_PC_SLOT_TEXT);
            }
            
            if (newTeam != UNASSIGNED_TEAM_INDEX) RegisterToTeamSlotLocal(user.ClientId, newTeam);
        }
        
        private void ResetTeamSlot(int teamIndex)
        {
            TeamData teamData = _teams[teamIndex];
            teamData.pcPlayerOwnerClientId = UNASSIGNED_TEAM_INDEX;
            _teams[teamIndex] = teamData;
        }

        public bool IsTeamPlayerSlotAvailable(int teamIndex)
        {
            if (teamIndex == UNASSIGNED_TEAM_INDEX) return true;
            
            if (TryGetTeam(teamIndex, out TeamData teamData))
            {
                return teamData.pcPlayerOwnerClientId == UNASSIGNED_TEAM_INDEX;
            }

            return false;
        }

        public bool IsTeamIndexValid(int teamIndex)
        {
            // Valid if teamIndex is in the range of the  or is unassigned
            return (teamIndex >= 0 && teamIndex < _teams.Length) || teamIndex == UNASSIGNED_TEAM_INDEX;
        }
        
        private void OnTeamSet(int teamIndex, string playerName)
        {
            OnTeamSetLocal(teamIndex, playerName);
            OnTeamSetClientRpc(teamIndex, playerName);
        }
        
        [ClientRpc]
        private void OnTeamSetClientRpc(int teamIndex, string playerName)
        {
            OnTeamSetLocal(teamIndex, playerName);
        }

        private void OnTeamSetLocal(int teamIndex, string playerName)
        {
            onTeamSet?.Invoke(teamIndex, playerName);
        }

        public TeamData[] GetTeamsData()
        {
            // return _teams.Where((t, i) => IsTeamPlayerSlotAvailable(i, PlayerPlatform.Pc) == false).ToArray();
            return _teams;
        }
        
        public TeamData GetTeamData(int teamIndex)
        {
            return _teams[teamIndex];
        }
        
        private void OnUserInstanceDespawned_CleanTeam(UserInstance userInstance)
        {
            if (userInstance.Team != UNASSIGNED_TEAM_INDEX)
            {
                TeamData teamData = _teams[userInstance.Team];
                teamData.pcPlayerOwnerClientId = UNASSIGNED_TEAM_INDEX;
                _teams[userInstance.Team] = teamData;
            }
        }
    }
}
