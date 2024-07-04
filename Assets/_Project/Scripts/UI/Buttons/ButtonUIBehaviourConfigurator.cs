using Project.Extensions;
using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;

namespace Project
{
    [RequireComponent(typeof(ButtonUIVisualConfigurator))]
    public class ButtonUIBehaviourConfigurator : MonoBehaviour, IPointerEnterHandler, IPointerClickHandler,
        IPointerExitHandler
    {
        #region Variables

        [Header("References")] private ButtonUIVisualConfigurator _buttonUIVisualConfigurator;

        [Title("State")] [SerializeField] private EButtonState _defaultState = EButtonState.Normal;
        [SerializeField] [ReadOnly] private EButtonState _currentState;

        [Title("Events")] [SerializeField] private UnityEvent _onButtonNormalEvent = new UnityEvent();
        [SerializeField] private UnityEvent _onButtonHighlightedEvent = new UnityEvent();
        [SerializeField] private UnityEvent _onButtonClickedEvent = new UnityEvent();

        [Space(30)] [SerializeField] [InfoBox("Which buttons should it impact")]
        private ButtonUIBehaviourConfigurator[] _buttonUIBehaviourConfigurators;

        #endregion


        #region Updates

        private void Awake()
        {
            _buttonUIVisualConfigurator = GetComponent<ButtonUIVisualConfigurator>();
        }


        private void Start()
        {
            _currentState = _defaultState;
            _buttonUIVisualConfigurator.SetVisual(_currentState);
        }

        #endregion


        #region Methods

        public void OnPointerEnter(PointerEventData eventData)
        {
            HighlightedStateBehaviour(true, false);
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            NormalStateBehaviour(true, false);
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            ClickedBehaviour(true);
        }


        public void NormalStateBehaviour(bool notify, bool forceState)
        {
            if (_currentState == EButtonState.Normal) return;
            if (_currentState == EButtonState.Clicked && forceState == false) return;

            _currentState = EButtonState.Normal;
            _buttonUIVisualConfigurator.SetVisual(_currentState);

            if (notify) _onButtonNormalEvent?.Invoke();
        }

        public void HighlightedStateBehaviour(bool notify, bool forceState)
        {
            if (_currentState == EButtonState.Highlighted) return;
            if (_currentState == EButtonState.Clicked && forceState == false) return;

            _currentState = EButtonState.Highlighted;
            _buttonUIVisualConfigurator.SetVisual(_currentState);

            if (notify) _onButtonHighlightedEvent?.Invoke();
        }

        public void ClickedBehaviour(bool notify)
        {
            if (_currentState == EButtonState.Clicked) return;

            _currentState = EButtonState.Clicked;
            _buttonUIVisualConfigurator.SetVisual(_currentState);
            _buttonUIBehaviourConfigurators.ForEach(x => x.NormalStateBehaviour(true, true));

            if (notify) _onButtonClickedEvent?.Invoke();
        }
        #endregion
    }
}