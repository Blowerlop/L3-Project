using DG.Tweening;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

namespace _Project._200_Dev.Menu
{
    public class MenuButtonManager : MonoBehaviour
    {
        public Button GameButton, TutorialButton, QuitButton, SettingsButton, ValidateButton;
        public GameObject playerNameSetter, connectionPopup;
        
        private Tween _delayTween;
        
        private void Start()
        {
            GameButton.onClick.AddListener(ShowPlayerNameInputField);
            TutorialButton.onClick.AddListener(PlayTutorial);
            QuitButton.onClick.AddListener(QuitGame);
            SettingsButton.onClick.AddListener(OpenSettings);
            ValidateButton.onClick.AddListener(JoinGame);
        }
        

        void ShowPlayerNameInputField()
        {
            playerNameSetter.SetActive(true);
        }
        
        void JoinGame()
        {
            playerNameSetter.SetActive(false);
            connectionPopup.SetActive(true);
            if (_delayTween == null) _delayTween = DOVirtual.DelayedCall(2.0f, () => NetworkManager.Singleton.StartClient()).OnComplete(() => _delayTween = null); 
        }

        void GoToLobby()
        {
            UnityEngine.SceneManagement.SceneManager.LoadScene("Lobby", LoadSceneMode.Single);
        }
        void PlayTutorial()
        {
            UnityEngine.SceneManagement.SceneManager.LoadScene("Tutorial", LoadSceneMode.Single);
        }

        void OpenSettings()
        {
            MenuManager.instance.Toggle();
        }
        void QuitGame()
        {
            Application.Quit();
        }
    }
}
