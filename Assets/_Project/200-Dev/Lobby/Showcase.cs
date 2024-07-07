using System.Globalization;
using System.Linq;
using _Project._000_Game.Constants;
using _Project._200_Dev.Entities;
using _Project._200_Dev.Entities.Player.Stats;
using _Project._200_Dev.Spells;
using _Project._200_Dev.Utilities.Extensions;
using Sirenix.OdinInspector;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace _Project._200_Dev.Lobby
{
    public class Showcase : MonoBehaviour
    {
        [SerializeField] private SOCharacter _characterData;

        [Title("Name")]
        [SerializeField] private TMP_Text _name;

        [Title("Spells")]
        [SerializeField] private Image _spell1;
        [SerializeField] private Image _spell2;
        [SerializeField] private Image _spell3;
        [SerializeField] private Image _spell4;
        [SerializeField] private SpellTooltip _spellTooltip1, _spellTooltip2, _spellTooltip3, _spellTooltip4; 

        [Title("Stats")] 
        [SerializeField] private TMP_Text _damage;
        [SerializeField] private TMP_Text _speed;
        [SerializeField] private TMP_Text _range;
        [SerializeField] private TMP_Text _health;
        
        [Title("Other")]
        [SerializeField] private GameObject _previewSpawnPoint;
        [SerializeField] private Image _background;


        private void OnValidate()
        {
            if (_characterData == null) return;
            
            _name.text = _characterData.characterName;
            
            var spells = _characterData.GetSpells();
            _spell1.sprite = spells[0].spellIcon;
            _spell2.sprite = spells[1].spellIcon;
            _spell3.sprite = spells[2].spellIcon;
            _spell4.sprite = spells[3].spellIcon;
            _spellTooltip1.UpdateToolTipText(spells[0]);
            _spellTooltip2.UpdateToolTipText(spells[1]);
            _spellTooltip3.UpdateToolTipText(spells[2]);
            _spellTooltip4.UpdateToolTipText(spells[3]);
            _damage.text = ((AttackDamageStat)_characterData.stats.First(x => x is AttackDamageStat)).value.ToString();
            _speed.text = ((MoveSpeedStat)_characterData.stats.First(x => x is MoveSpeedStat)).value.ToString();
            _range.text = ((AttackRangeStat)_characterData.stats.First(x => x is AttackRangeStat)).value.ToString(CultureInfo.InvariantCulture);
            _health.text = ((HealthStat)_characterData.stats.First(x => x is HealthStat)).maxValue.ToString();
            
            _background.sprite = _characterData.avatar2;
            
            #if UNITY_EDITOR
            UnityEditor.EditorApplication.delayCall += () =>
            #endif
            {
                _previewSpawnPoint.DestroyChildren();
                var modelInstance = Instantiate(_characterData.model, _previewSpawnPoint.transform);
                modelInstance.GetComponentsInChildren<Transform>().ForEach(x => x.gameObject.layer = Constants.Layers.EntityIndex); 
            };
        }

        public void UpdateData(SOCharacter characterData)
        {
            if (characterData == _characterData) return;
            
            _characterData = characterData;
            OnValidate();
        }

    }
}
