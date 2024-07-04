using Cysharp.Threading.Tasks;
using Project._Project.Scripts;
using Project._Project.TESTT_REBIND;
using Project.Spells;
using Project.Spells.Casters;
using System;
using System.Threading;
using TMPro;
using Unity.Netcode;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

namespace Project
{
    public class TutorialSequencer : MonoBehaviour
    {
        #region Public
        public static TutorialSequencer instance;
        public AudioHelper audioHelper; 
        public GameObject MobilePlayer;
        public GameObject coneGO; 
        public Transform tutorialMvtCheck;
        public Button SkipTutoButton;
        public TextMeshProUGUI SkipButtonText; 
        #endregion


        #region Privates

        [SerializeField] SOCharacter tutorialCharacter;
        CancellationTokenSource cts;
        UserInstance userInstance;
        PCPlayerRefs PcRefs;
        Canvas SpellCanvas; 
        [SerializeField] Dummy tutorialDummy;
        [SerializeField] TextMeshProUGUI subtitleTmp;

        [Header("Input")]
        [SerializeField] InputActionReference spell1;
        [SerializeField] InputActionReference spell2;
        [SerializeField] InputActionReference spell3;
        [SerializeField] InputActionReference spell4;

        const int timeOutDelay = 5000;

        #endregion
        private void Start()
        {
            instance = this; 
            SkipTutoButton.onClick.AddListener(QuitTutorial);
            AudioHelper.OnTimestampReached += ReactToMessage;
            Subtitle.OnWrite += UpdateSubtitleText;
            _ = PlayTutorial();
        }

        void OnDestroy()
        {
            instance = null; 
            SkipTutoButton.onClick.RemoveAllListeners();
            AudioHelper.OnTimestampReached -= ReactToMessage;
            Subtitle.OnWrite += UpdateSubtitleText;
        }
        async UniTask PlayTutorial()
        {
            try
            {
                await LoadTutorialScene();

                //Intro
                Subtitle.StartWriting(null, "Welcome to Convergence ! I'm going to guide you through the game mechanics." +
                    "This game is played in pairs, a PC player represented by the Ranger, and a Mobile Player represented by the Fairy." +
                    "As a PC Player, you have the major role of controlling movements and attacks. The Mobile Player will act as a support," +
                    " but you'll be in charge of all the moves.");
                await audioHelper.PlayAsync("Introduction", gameObject.GetCancellationTokenOnDestroy());

                await CheckMovementSequence();

                await CheckAttackSequence();

                await CheckSpellSequence();

                Subtitle.StartWriting(null, "Currently the camera is locked on your character by default. To lock or unlock your camera, <color=#e3d044> double tap on your spacebar </color>.");
                await audioHelper.PlayAsync("Camera", gameObject.GetCancellationTokenOnDestroy());

                await UniTask.Delay(1500);

                Subtitle.StartWriting(null, "A round is won when you manage to reduce your opponent's health point to 0. A game ends when a team has won two rounds.");

                await audioHelper.PlayAsync("WinCond", gameObject.GetCancellationTokenOnDestroy());

                await UniTask.Delay(1500);

                Subtitle.StartWriting(null, "You now have all the information you need to fight. This tutorial has only introduced you to one character, " +
                    "don't hesitate to try the others and train your synergy with your mobile ally.");

                await audioHelper.PlayAsync("Conclusion", gameObject.GetCancellationTokenOnDestroy());


                SkipButtonText.text = "LEAVE"; 
                //await UniTask.Delay(1000);
                //QuitTutorial();
            }

            catch (Exception ex)
            {
                Debug.LogWarning(ex.Message);
            }
        }

        
        public void QuitTutorial()
        {
            Netcode_ConnectionManager.Disconnect();
            UnityEngine.SceneManagement.SceneManager.LoadScene("Menu", LoadSceneMode.Single);
        }


        #region Tuto Spell

        async UniTask CheckSpellSequence()
        {
            Subtitle.StartWriting(null, "Each character has 4 unique spells. You can use them respectively by pressing the <color=#e3d044>" + InputSettingsManager.GetBindingName(spell1.action.name, 0) +
                ", " + InputSettingsManager.GetBindingName(spell2.action.name, 0) + ", " + InputSettingsManager.GetBindingName(spell3.action.name, 0) + " and " + InputSettingsManager.GetBindingName(spell4.action.name, 0) 
                +"</color> keys. Try them out on the dummy as much as you like. Hover over the spells to read informations on them.");
            await audioHelper.PlayAsync("Spell", gameObject.GetCancellationTokenOnDestroy());

            await UniTask.Delay(1500);

            Debug.Log("<color=Yellow> Spell OK </color>");
        }

        #endregion

        #region Tuto Attack

        async UniTask CheckAttackSequence()
        {
            Subtitle.StartWriting(null, "Great ! To attack, use your <color=#e3d044> right mouse button </color> while aiming at an opponent. Try to attack the dummy. ");
            await audioHelper.PlayAsync("Attack", gameObject.GetCancellationTokenOnDestroy());

            await UniTask.WaitUntil(() => userInstance.LinkedPlayer.NetworkAnimator.Animator.GetCurrentAnimatorStateInfo(0).fullPathHash == -1146545271);

            coneGO.SetActive(false);

            await UniTask.Delay(1500);

            Debug.Log("<color=Yellow> Attack OK </color>");
        }

        void SpawnDummy()
        {
            tutorialDummy = Instantiate(tutorialDummy, new Vector3(5, 0, 5), Quaternion.identity);
            tutorialDummy.GetComponent<NetworkObject>().Spawn();

            coneGO.SetActive(true);
            coneGO.transform.SetParent(tutorialDummy.transform);
            coneGO.transform.localScale = 0.2f * Vector3.one;
            coneGO.transform.localPosition = new Vector3(0, 2f, 0);
        }
        #endregion

        #region Tuto Movement

        async UniTask CheckMovementSequence()
        {
            Subtitle.StartWriting(null, "To move, use the <color=#e3d044> right click button</color>. Try to move to the highlighted point. ");
            await audioHelper.PlayAsync("Movement", gameObject.GetCancellationTokenOnDestroy());

            await UniTask.WaitUntil(() => (userInstance.LinkedPlayer.transform.position - tutorialMvtCheck.position).sqrMagnitude < 5f);
            
            Debug.Log("<color=Yellow> Movement OK </color>");

            tutorialMvtCheck.gameObject.SetActive(false);
        }

        #endregion

        #region Loading Scene
        async UniTask LoadTutorialScene()
        {
            try
            {
                StartHost();

                cts = new CancellationTokenSource(timeOutDelay);
                await UniTask.WaitUntil(() => NetworkManager.Singleton.IsServer, PlayerLoopTiming.Update, cts.Token);

                cts.Dispose();
                cts = new CancellationTokenSource(timeOutDelay);
                ValidateCharacterServerRpcc();


                await UniTask.WaitUntil(() => UserInstanceManager.instance.GetUserInstance((int)NetworkManager.Singleton.LocalClientId).CharacterId != 0, PlayerLoopTiming.Update, cts.Token);
                userInstance = UserInstanceManager.instance.GetUserInstance((int)NetworkManager.Singleton.LocalClientId);
                cts.Dispose();

                LoadScene();


                await UniTask.WaitUntil(() => userInstance != null && userInstance.LinkedPlayer != null, PlayerLoopTiming.Update);

                //Attach Mobile
                MobilePlayer.transform.SetParent(userInstance.LinkedPlayer.transform);
                MobilePlayer.transform.localPosition = new Vector3(1.5f, 2, 0);


                //Disable Spells
                PcRefs = (PCPlayerRefs)userInstance.LinkedPlayer;
                Destroy(PcRefs.SpellCastController);
                SpellCanvas = FindObjectOfType<SpellUI>().transform.root.GetComponent<Canvas>() ;
                SpellCanvas.enabled = false;


                Debug.Log("<color=Yellow> Loading Tutorial OK </color>");
            }
            catch (Exception ex)
            {
                cts?.Dispose();
                Debug.Log("Error in tutorial set up, back to menu - " + ex.Message);
                SceneManager.Network_LoadSceneAsync("Menu", LoadSceneMode.Single, new LoadingScreenParameters(null, Color.black));
            }
        }
        void StartHost() => Netcode_ConnectionManager.StartHost();

        public void LoadScene()
        {
            SceneManager.Network_LoadSceneAsync("SpellAdditive", LoadSceneMode.Additive, new LoadingScreenParameters(null, Color.black));
        }

        public void ValidateCharacterServerRpcc() => ValidateCharacterServerRpc((int)NetworkManager.Singleton.LocalClientId, tutorialCharacter.id);


        [ServerRpc(RequireOwnership = false)]
        private void ValidateCharacterServerRpc(int clientId, int characterId)
        {
            UserInstance userInstance = UserInstanceManager.instance.GetUserInstance(clientId);
            if (userInstance.CharacterId == characterId) return;

            userInstance.SrvSetCharacter(characterId);
        }


        #endregion

        void UpdateSubtitleText(string currentPhrase)
        {
            subtitleTmp.text = currentPhrase;
        }

        void ReactToMessage(string message)
        {

            switch (message)
            {
                case "PCPlayerIndication":
                    coneGO.SetActive(true);
                    coneGO.transform.SetParent(PcRefs.transform);
                    coneGO.transform.localScale = 0.4f * Vector3.one;
                    coneGO.transform.localPosition = new Vector3(0, 5.49f, 0);
                    break;
                case "MobilePlayerIndication":
                    coneGO.transform.SetParent(MobilePlayer.transform);
                    coneGO.transform.localScale = 0.2f * Vector3.one;
                    coneGO.transform.localPosition = new Vector3(0, 2f, 0);
                    break;
                case "HideCone":
                    coneGO.SetActive(false);
                    break;
                case "HighlightMovement":
                    tutorialMvtCheck.gameObject.SetActive(true);
                    break;
                case "SpawnDummy":
                    SpawnDummy();
                    break;
                case "EnableSpells":
                    SpellCanvas.enabled = true;
                    PcRefs.gameObject.AddComponent<SpellCastController>().Init(PcRefs);
                    break;
                case "ZeroHealth":
                    break;
                case "RoundWin":
                    break;
                default:
                    break;
            }
        }
    }
}
