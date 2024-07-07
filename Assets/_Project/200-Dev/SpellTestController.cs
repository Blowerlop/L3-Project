using Sirenix.OdinInspector;
using UnityEngine;

namespace Project._Project.Scripts
{
    public class SpellTestController : MonoBehaviour
    {
        [Button("Set Character Data")]
        public void SetCharacter(int clientId, SOCharacter data)
        {
            UserInstanceManager.instance.GetUserInstance(clientId).SrvSetCharacter(data.id);
        }
        
        [Button("Set Team")]
        public void SetTeam(int clientId, int teamId)
        {
            TeamManager.instance.TrySetTeam(clientId, teamId);
        }
    }
}