using _Project._200_Dev.User;
using Unity.Netcode;
using UnityEngine;
using Button = _Project._200_Dev.UI_Framework.Runtime.Button;

namespace _Project._200_Dev.Lobby
{
    public class OnTeamSet_ToggleReadyButtonVisibility : NetworkBehaviour
    {
        [SerializeField] private Button _button;
        
        
        public override void OnNetworkSpawn()
        {
            OnTeamSet_ToggleButtonVisibility(TeamManager.UNASSIGNED_TEAM_INDEX, TeamManager.UNASSIGNED_TEAM_INDEX);
            Utilities.Utilities.StartWaitUntilAndDoAction(this, () => UserInstance.Me != null, () => UserInstance.Me._networkTeam.OnValueChanged += OnTeamSet_ToggleButtonVisibility);
        }

        public override void OnNetworkDespawn()
        {
            if (UserInstance.Me != null) UserInstance.Me._networkTeam.OnValueChanged -= OnTeamSet_ToggleButtonVisibility;
        }
        
        
        private void OnTeamSet_ToggleButtonVisibility(int previousTeam, int currentTeam)
        {
            _button.SetVisibility(currentTeam != TeamManager.UNASSIGNED_TEAM_INDEX);
        }
    }
}
