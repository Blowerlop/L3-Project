using TMPro;
using UnityEngine;

namespace _Project._200_Dev.Menu
{
    public class PlayerNameSetterUI : MonoBehaviour
    {
        [SerializeField] private TMP_InputField _inputField;


        private void Start()
        {
            _inputField.text = PlayerData.playerName;
        }

        private void OnEnable()
        {
            _inputField.onEndEdit.AddListener(SetPlayerName);
            Utilities.Utilities.StartWaitForFramesAndDoActionCoroutine(this, 1, () =>
            {
                _inputField.ActivateInputField();
                _inputField.MoveTextEnd(false);
            });
        }
        
        private void OnDisable()
        {
            _inputField.onEndEdit.RemoveListener(SetPlayerName);
        }

        private void SetPlayerName(string name)
        {
            Debug.Log("Setting player name to: " + name);
            PlayerData.playerName = name;
        }
    }
}
