using System.Threading;
using _Project._200_Dev.User;
using Cysharp.Threading.Tasks;
using TMPro;
using UnityEngine;

namespace _Project._200_Dev.Entities.Player
{
    public class PCPlayerPseudo : MonoBehaviour
    {
        [SerializeField] PCPlayerRefs playerRefs;
        public TextMeshProUGUI playerPseudoText;
        CancellationTokenSource cts; 
        void Start()
        {
           _ = Initialize(); 
        }

        async UniTask Initialize()
        {
            cts = new CancellationTokenSource(5000);
            var users = UserInstanceManager.instance.GetUsersInstance();

            await UniTask.WaitUntil(() => playerRefs.TeamIndex != -1, PlayerLoopTiming.FixedUpdate, cts.Token);

            cts.Dispose();
            for (int i = 0; i < users.Length; i++)
            {
                if (users[i].Team == playerRefs.TeamIndex)
                {
                    playerPseudoText.text = users[i].PlayerName;
                    return;
                }
            }

            playerPseudoText.text = string.Empty;
            
        }

    }
}   