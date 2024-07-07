using System.Globalization;
using _Project._200_Dev.Entities;
using _Project._200_Dev.Entities.Player;
using _Project._200_Dev.Spells.Cooldowns;
using _Project._200_Dev.User;
using DG.Tweening;
using TMPro;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.UI;

namespace _Project._200_Dev.Spells.Spells
{
    public class SpellUI : MonoBehaviour
    {
        private CooldownController _cooldowns;
        [SerializeField] private int id;

        [SerializeField] private CanvasGroup group;
        [SerializeField] private TextMeshProUGUI tmp;
        [SerializeField] private Image cooldownFilter;
        [SerializeField] private Image spellIcon;
        [SerializeField] private SpellTooltip spellTooltip; 

        private PCPlayerRefs _localPlayer;
        
        private float _maxTime;
        
        private void Awake()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            
            UserInstance.Me.OnPlayerLinked += Setup;
        }

        private void OnDestroy()
        {
            if (NetworkManager.Singleton is { IsClient: false }) return;
            if (UserInstance.Me != null) UserInstance.Me.OnPlayerLinked -= Setup;
            
            if (!_cooldowns) return;
            
            // For clients only
            _cooldowns.OnLocalCooldownStarted -= OnCooldownStarted;
            _cooldowns.OnLocalCooldownUpdated -= OnCooldownUpdated;
            
            // For host only
            _cooldowns.OnServerCooldownStarted -= OnCooldownStarted;
            _cooldowns.OnServerCooldownUpdated -= OnCooldownUpdated;
            
            _cooldowns.OnServerCooldownEnded -= OnCooldownEnded;
        }

        private void Setup(PlayerRefs refs)
        {
            _cooldowns = refs.Cooldowns;

            if (!NetworkManager.Singleton.IsHost)
            {
                _cooldowns.OnLocalCooldownStarted += OnCooldownStarted;
                _cooldowns.OnLocalCooldownUpdated += OnCooldownUpdated;
            }
            else
            {
                _cooldowns.OnServerCooldownStarted += OnCooldownStarted;
                _cooldowns.OnServerCooldownUpdated += OnCooldownUpdated;
            }
            
            _cooldowns.OnServerCooldownEnded += OnCooldownEnded;

            group.alpha = 0;

            if (SOCharacter.TryGetCharacter(UserInstanceManager.instance.GetUserInstance((int) NetworkManager.Singleton.LocalClientId).CharacterId, out var character))
            {
                if (character.TryGetSpell(id, out var spellData))
                {
                    spellIcon.sprite = spellData.spellIcon;
                    spellTooltip?.UpdateToolTipText(spellData);
                }
            }
            
        }
        
        private void OnCooldownStarted(int index, float time)
        {
            if (index != id) return;
            
            group.DOFade(1, 0);
            cooldownFilter.fillAmount = 1;
            tmp.text = Mathf.RoundToInt(time).ToString();

            _maxTime = time;
        }

        private void OnCooldownUpdated(int index, float value)
        {
            if (index != id) return;
            
            tmp.text = value.ToString(CultureInfo.InvariantCulture);
            cooldownFilter.fillAmount = value / _maxTime;
        }
        
        private void OnCooldownEnded(int index)
        {
            if (index != id) return;
            
            group.DOFade(0, 0);
        }

        public void OnHover()
        {
            if (!TryGetLocalPlayer(out var player)) return;
            
            player.SpellCastController.PreviewSpell(id);
        }

        public void OnUnHover()
        {
            if (!TryGetLocalPlayer(out var player)) return;
            
            player.SpellCastController.StopSpellPreview(id);
        }

        private bool TryGetLocalPlayer(out PCPlayerRefs refs)
        {
            refs = _localPlayer;
            if (_localPlayer) return true;
            
            var player = UserInstance.Me.LinkedPlayer;
            if (!player) return false;
            
            _localPlayer = player.GetPC();
            refs = _localPlayer;
            return true;
        }
    }
}