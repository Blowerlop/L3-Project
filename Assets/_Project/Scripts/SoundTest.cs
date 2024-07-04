using Project._Project.Scripts.Managers;
using UnityEngine;
using UnityEngine.UI;

namespace Project._Project.Scripts
{
    public class SoundTest : MonoBehaviour
    {
        public TMPro.TMP_Dropdown drop;

        public Button globalButton, instanceButton, playerActionButton, startSnapButton, stopSnapButton;

        public TMPro.TMP_InputField snapInputField;
        
        // Start is called before the first frame update
        void Start()
        {
            var mana = SoundManager.instance;
            drop.AddOptions(mana.GetAllEvent());
            globalButton.onClick.AddListener(() =>
                mana.PlayGlobalSound(drop.options[drop.value].text, "test", SoundManager.EventType.Music));
            instanceButton.onClick.AddListener(() =>
                mana.PlayStaticSound(drop.options[drop.value].text, "test", Vector3.up, SoundManager.EventType.Music));
            //globalButton.onClick.AddListener(() =>
            //    mana.AddPlayerActionSound(drop.options[drop.value].text, "test", SoundManager.EventType.Music));
            startSnapButton.onClick.AddListener(() =>
                mana.TriggerSnapshot(snapInputField.text, "test"));
            stopSnapButton.onClick.AddListener(() =>
                mana.StopSnapshot("test"));
        }

        // Update is called once per frame
        void Update()
        {
        
        }
    }
}
