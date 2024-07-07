using _Project._200_Dev.User;
using UnityEngine;

namespace _Project._200_Dev.UI
{
    public class TeamHeader : MonoBehaviour
    {
        [SerializeField] private TeamHeaderItem prefab;
        [SerializeField] private Transform prefabParent;
        
        private void Start()
        {
            foreach(var user in UserInstanceManager.instance.GetUsersInstance())
            {
                var teamIndex = user.Team;

                var teamHeader = Instantiate(prefab, prefabParent);
                teamHeader.Init(user, teamIndex);
            }
        }
    }
}
