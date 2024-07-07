using DG.Tweening;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace Project
{
    public class EmotesWheelItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler
    {
        private EmotesWheelUI _emotesWheelUI;
        
        [SerializeField] private Image emoteImage; 
        
        [SerializeField] private CanvasGroup highlightGroup;

        [SerializeField] private float highlightDuration = 0.2f;

        private int _index;
        private Sequence _sequence;
        
        public int Index => _index;

        private void OnEnable()
        {
            highlightGroup.alpha = 0;
        }

        private void Start()
        {
            highlightGroup.alpha = 0;
        }

        public void Init(EmotesWheelUI wheel, EmoteData data, int index)
        {
            _index = index;
            _emotesWheelUI = wheel;
            
            emoteImage.sprite = data.EmoteSprite;
        }
        
        public void OnPointerEnter(PointerEventData eventData)
        {
            if(_sequence != null && _sequence.IsActive()) _sequence.Kill();
            _sequence = DOTween.Sequence();
            
            _sequence.Append(highlightGroup.DOFade(1, highlightDuration));
            
            _emotesWheelUI.SetItemSelected(this);
        }

        public void OnPointerExit(PointerEventData eventData)
        {           
            if(_sequence != null && _sequence.IsActive()) _sequence.Kill();
            _sequence = DOTween.Sequence();
            
            _sequence.Append(highlightGroup.DOFade(0, highlightDuration));
            
            _emotesWheelUI.SetItemSelected(null);
        }
    }
}