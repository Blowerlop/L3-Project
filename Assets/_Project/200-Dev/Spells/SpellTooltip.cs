using Project._Project.TESTT_REBIND;
using Project.Spells;
using TMPro;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.UI;

namespace Project
{
    public class SpellTooltip : MonoBehaviour
    {
        [SerializeField] Image spellImage; 
        [SerializeField] TextMeshProUGUI spellNameText, spellCooldownText, spellDescriptionText;
        public InputActionReference inputActionReference; 
        private string spellName = "" ; 
        private void Start()
        {
            gameObject.SetActive(false);
            if (inputActionReference == null) Debug.LogWarning("Please put a reference in Action input variable");
            InputSettingsManager.onRebindComplete += UpdateBinding;
        }

        private void OnDestroy()
        {
            InputSettingsManager.onRebindComplete -= UpdateBinding;
        }
        public void UpdateToolTipText(SpellData spellData)
        {
            spellName = spellData.spellName; 
            if (inputActionReference != null ) spellNameText.text = "[" + InputSettingsManager.GetBindingName(inputActionReference.action.name, 0) + "] " + spellName;
            spellCooldownText.text = spellData.cooldown.ToString() + "s" ;
            spellDescriptionText.text = spellData.spellDescription.GenerateText();
            spellImage.sprite = spellData.spellIcon; 
        }

        public void UpdateBinding()
        {
            if (inputActionReference != null) spellNameText.text = "[" + InputSettingsManager.GetBindingName(inputActionReference.action.name, 0) + "] " + spellName;
        }

    }
}
