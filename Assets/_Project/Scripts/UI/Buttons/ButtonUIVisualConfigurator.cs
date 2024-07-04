using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.UI;

namespace Project
{
    public enum EButtonState
    {
        Normal,
        Highlighted,
        Clicked,
    }

    [RequireComponent(typeof(ButtonUIBehaviourConfigurator))]
    public class ButtonUIVisualConfigurator : MonoBehaviour
    {
        #region Variables

        [Title("References")] 
        [SerializeField, FoldoutGroup("References")] private GameObject _imagesContainer;
        [SerializeField, FoldoutGroup("References")] private Image _baseImage;
        [SerializeField, FoldoutGroup("References")] private Image _outlineImage;
        [SerializeField, FoldoutGroup("References")] private TMPro.TMP_Text _text;
        [SerializeField, FoldoutGroup("References")] private RectTransform _spacer;

        [Space(30)] [Title("Global Settings")] 
        [SerializeField] private bool _addSpaceBetweenImagesAndText;
        [SerializeField, ShowIf("_addSpaceBetweenImagesAndText")] private float _spaceBetweenImagesAndText = 0.0f;
        [SerializeField] private bool _invertTextAndImage;
        [SerializeField] [MinValue(0)] private float _imageSize = 30.0f;

        // Base Image
        [Space(30)] [Title("Base Image")] [SerializeField]
        private bool _useImage = true;

        [Space(15)] [SerializeField] [ShowIf("_useImage")]
        private Sprite _baseNormalSprite;

        [SerializeField] [ShowIf("_useImage")] private Color _baseNormalColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        [Space(10)] [SerializeField] [ShowIf("@this._useImage && this._hasHighlightedState")]
        private Sprite _baseHighlightedSprite;

        [SerializeField] [ShowIf("@this._useImage && this._hasHighlightedState")]
        private Color _baseHighlightedColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        [Space(10)] [SerializeField] [ShowIf("@this._useImage && this._hasClickedState")]
        private Sprite _baseClickedSprite;

        [SerializeField] [ShowIf("@this._useImage && this._hasClickedState")]
        private Color _baseClickedColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        // Outline Image
        [Title("Outline Image")] [Space(30)] [SerializeField]
        private bool _useOutline = true;

        [SerializeField] [ShowIf("_useOutline")]
        private Sprite _outlineNormalSprite;

        [SerializeField] [ShowIf("_useOutline")]
        private Color _outlineNormalColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        [Space(10)] [SerializeField] [ShowIf("@this._useOutline && this._hasHighlightedState")]
        private Sprite _outlineHighlightedSprite;

        [SerializeField] [ShowIf("@this._useOutline && this._hasHighlightedState")]
        private Color _outlineHighlightedColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        [Space(10)] [SerializeField] [ShowIf("@this._useOutline && this._hasClickedState")]
        private Sprite _outlineClickedSprite;

        [SerializeField] [ShowIf("@this._useOutline && this._hasClickedState")]
        private Color _outlineClickedColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        // Text
        [Space(30)] [Title("Text")] [SerializeField]
        private bool _useText = true;

        [SerializeField] [ShowIf("_useText")] private string _textContent;
        [SerializeField] [ShowIf("_useText")] private Color _textNormalColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        [SerializeField] [ShowIf("@this._useText && this._hasHighlightedState")]
        private Color _textHighlightedColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        [SerializeField] [ShowIf("@this._useText && this._hasClickedState")]
        private Color _textClickedColor = new Color(255.0f, 255.0f, 255.0f, 1.0f);

        // States
        [Space(30)] [Title("States")] [SerializeField]
        private bool _hasHighlightedState;

        [SerializeField] private bool _hasClickedState;
        [SerializeField] private EButtonState _previewState = EButtonState.Normal;

        #endregion


        #region Updates

        private void OnValidate()
        {
            if (gameObject.activeSelf == false) return;
            bool needToReturned = false;

            if (_imagesContainer == null)
            {
                Debug.LogWarning($"{name} : ImagesParent is null !");
                needToReturned = true;
            }

            if (_baseImage == null)
            {
                Debug.LogError($"{name} : BaseImage is null !");
                needToReturned = true;
            }

            if (_outlineImage == null)
            {
                Debug.LogError($"{name} : OutlineImage is null !");
                needToReturned = true;
            }

            if (_text == null)
            {
                Debug.LogError($"{name} : Text is null !");
                needToReturned = true;
            }

            if (_spacer == null)
            {
                Debug.LogError($"{name} : Spacer is null !");
                needToReturned = true;
            }

            if (needToReturned)
            {
                Debug.Log("C'est ce gameObject le coupable : " + gameObject.name);
                return;
            }

            if (Application.isPlaying) return;

            // Set active state of image / outline / text
            _imagesContainer.SetActive(_useImage);
            _outlineImage.gameObject.SetActive(_useOutline);
            _text.gameObject.SetActive(_useText);

            // Set the distance between the image and the text
            if (_addSpaceBetweenImagesAndText)
            {
                _spacer.sizeDelta = new Vector2(_spaceBetweenImagesAndText, _spacer.sizeDelta.y);

                _imagesContainer.GetComponent<RectTransform>().sizeDelta = new Vector2(_imageSize,
                    _imagesContainer.GetComponent<RectTransform>().sizeDelta.y);
                _text.text = _textContent;
            }

            if (_invertTextAndImage)
            {
                _imagesContainer.transform.SetAsLastSibling();
                _text.transform.SetAsFirstSibling();
            }
            else
            {
                _imagesContainer.transform.SetAsFirstSibling();
                _text.transform.SetAsLastSibling();
            }

            // Preview the visual depending on the state
            switch (_previewState)
            {
                // Normal,
                // Highlighted,
                // Clicked,

                case EButtonState.Normal:
                    SetNormalVisual();
                    break;

                case EButtonState.Highlighted:
                    if (_hasHighlightedState == false)
                    {
                        _previewState = EButtonState.Normal;
                        break;
                    }

                    SetHighlightedVisual();
                    break;

                case EButtonState.Clicked:
                    if (_hasClickedState == false)
                    {
                        _previewState = EButtonState.Normal;
                        break;
                    }

                    SetClickedVisual();
                    break;
            }
        }

        #endregion


        #region Methods

        public void SetVisual(EButtonState buttonState)
        {
            switch (buttonState)
            {
                case EButtonState.Normal:
                    SetNormalVisual();
                    break;

                case EButtonState.Highlighted:
                    SetHighlightedVisual();
                    break;

                case EButtonState.Clicked:
                    SetClickedVisual();
                    break;
            }
        }

        private void SetNormalVisual()
        {
            _previewState = EButtonState.Normal;

            _baseImage.sprite = _baseNormalSprite;
            _baseImage.color = _baseNormalColor;

            _outlineImage.sprite = _outlineNormalSprite;
            _outlineImage.color = _outlineNormalColor;

            _text.color = _textNormalColor;
        }

        private void SetHighlightedVisual()
        {
            if (_hasHighlightedState == false || _previewState == EButtonState.Clicked) return;
            _previewState = EButtonState.Highlighted;

            _baseImage.sprite = _baseHighlightedSprite;
            _baseImage.color = _baseHighlightedColor;

            _outlineImage.sprite = _outlineHighlightedSprite;
            _outlineImage.color = _outlineHighlightedColor;

            _text.color = _textHighlightedColor;
        }

        private void SetClickedVisual()
        {
            if (_hasClickedState == false) return;
            _previewState = EButtonState.Clicked;

            _baseImage.sprite = _baseClickedSprite;
            _baseImage.color = _baseClickedColor;

            _outlineImage.sprite = _outlineClickedSprite;
            _outlineImage.color = _outlineClickedColor;

            _text.color = _textClickedColor;
        }

        #endregion
    }
}