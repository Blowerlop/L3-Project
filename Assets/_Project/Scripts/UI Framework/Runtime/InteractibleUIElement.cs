using Project._Project.Scripts.Managers;
using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.Serialization;


namespace Project.Scripts.UIFramework
{
    public abstract class InteractibleUIElement : MonoBehaviour, IPointerDownHandler, IPointerUpHandler, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
    {
        // Settings
        [TabGroup("Group", "Settings")] 
        [SerializeField, BoxGroup("Group/Settings/Sound")] private bool _playHoverSound = true;
        [SerializeField, BoxGroup("Group/Settings/Sound")] private bool _playClickSound = true;
        
        // Events
        [FormerlySerializedAs("_onHover")] [SerializeField, TabGroup("Group", "Events")] public UnityEvent onHover = new UnityEvent();
        [FormerlySerializedAs("_onUnHover")] [SerializeField, TabGroup("Group", "Events")] public UnityEvent onUnHover = new UnityEvent();
        [FormerlySerializedAs("_onClickDown")] [SerializeField, TabGroup("Group", "Events")] public UnityEvent onClickDown = new UnityEvent();
        [FormerlySerializedAs("_onClickUp")] [SerializeField, TabGroup("Group", "Events")] public UnityEvent onClickUp = new UnityEvent();
        [FormerlySerializedAs("_onClick")] [SerializeField, TabGroup("Group", "Events")] public UnityEvent onClick = new UnityEvent();
        
        [FormerlySerializedAs("_onHoverAnimation")] [SerializeField, FoldoutGroup("Group/Events/Animations")] public UnityEvent onHoverAnimation = new UnityEvent();
        [FormerlySerializedAs("_onUnHoverAnimation")] [SerializeField, FoldoutGroup("Group/Events/Animations")] public UnityEvent onUnHoverAnimation = new UnityEvent();
        [FormerlySerializedAs("_onClickDownAnimation")] [SerializeField, FoldoutGroup("Group/Events/Animations")] public UnityEvent onClickDownAnimation = new UnityEvent();
        [FormerlySerializedAs("_onClickUpAnimation")] [SerializeField, FoldoutGroup("Group/Events/Animations")] public UnityEvent onClickUpAnimation = new UnityEvent();
        [FormerlySerializedAs("_onClickAnimation")] [SerializeField, FoldoutGroup("Group/Events/Animations")] public UnityEvent onClickAnimation = new UnityEvent();
        


        protected virtual void OnEnable()
        {
            onUnHoverAnimation.Invoke();
        }
        
        protected virtual void OnDisable()
        {
            onUnHoverAnimation.Invoke();
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            if (_playHoverSound) PlayHoverSound();
            onHoverAnimation.Invoke();
            onHover.Invoke();
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            onUnHoverAnimation.Invoke();
            onUnHover.Invoke();
        }
        
        public virtual void OnPointerDown(PointerEventData eventData)
        {
            if (_playClickSound) PlayClickSound();
            onClickDownAnimation.Invoke();
            onClickDown.Invoke();
        }
        
        public virtual void OnPointerUp(PointerEventData eventData)
        {
            onClickUpAnimation.Invoke();
            onClickUp.Invoke();
        }
        
        public virtual void OnPointerClick(PointerEventData eventData)
        {
            onClickAnimation.Invoke();
            onClick.Invoke();
        }

        protected void PlayHoverSound() => SoundManager.instance.PlayGlobalSound("hover", "Menu", SoundManager.EventType.UI);
        protected void PlayClickSound() => SoundManager.instance.PlayGlobalSound("click", "Menu", SoundManager.EventType.UI);
    }
}