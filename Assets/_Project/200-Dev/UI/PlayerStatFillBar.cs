using _Project._200_Dev.Entities.Player;
using _Project._200_Dev.Entities.Player.Stats;
using _Project._200_Dev.User;
using Sirenix.OdinInspector;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.UI;

namespace _Project._200_Dev.UI
{
    public class PlayerStatFillBar<T> : FillBar where T : Stat<int>
    {
        [SerializeField] private bool getFromUserInstance = true;
        [SerializeField, HideIf(nameof(getFromUserInstance))] private PlayerRefs refs;

        [SerializeField] private bool useColors;

        [SerializeField, ShowIf(nameof(useColors))] private Graphic mainColoredImage, secondColoredImage; 
        
        [SerializeField, FoldoutGroup("Ally"), ShowIf(nameof(useColors))] 
        private Color allyMainColor, allySecondColor;
        
        [SerializeField, FoldoutGroup("Enemy"), ShowIf(nameof(useColors))] 
        private Color enemyMainColor, enemySecondColor;
        
        private PlayerStats _stats;
        
        private void Awake()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            
            if (getFromUserInstance) UserInstance.Me.OnPlayerLinked += Setup;
            else
            {
                Setup(refs);

                if (useColors)
                {
                    HandleColors(refs.TeamIndex);
                    refs.OnTeamChangedCallback += HandleColors;
                }
            }
            
            SetFillAmount(1);
        }

        private void OnDestroy()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            
            if (getFromUserInstance && UserInstance.Me != null) 
                UserInstance.Me.OnPlayerLinked -= Setup;
            
            if (useColors)
                refs.OnTeamChangedCallback -= HandleColors;
            
            if (!_stats) return;
            
            _stats.Get<T>().OnValueChanged -= OnValueChanged;
        }
        
        private void Setup(PlayerRefs refs)
        {
            if (refs is not PCPlayerRefs _refs) return;
            
            _stats = _refs.Entity.Stats;

            if (_stats.isInitialized) OnStatsInitialized();
            else _stats.OnStatsInitialized += OnStatsInitialized;
        }
        
        private void OnStatsInitialized()
        {
            var stat = _stats.Get<T>();
            OnValueChanged(stat.value, stat.maxValue);
            stat.OnValueChanged += OnValueChanged;
            
            _stats.OnStatsInitialized -= OnStatsInitialized;
        }
        
        protected virtual void OnValueChanged(int currentValue, int maxValue)
        {
            Debug.Log($"Current Value: {currentValue}, Max Value: {maxValue}");
            SetFillAmount(currentValue, maxValue);
        }

        private void HandleColors(int teamIndex)
        {
            if (!useColors) return;
            
            var isAlly = refs.TeamIndex == UserInstance.Me.Team;
            
            if (isAlly) SetColors(allyMainColor, allySecondColor);
            else SetColors(enemyMainColor, enemySecondColor);
        }

        private void SetColors(Color c0, Color c1)
        {
            mainColoredImage.color = c0;
            secondColoredImage.color = c1;
        }
    }
}