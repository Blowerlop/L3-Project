using System;
using System.Collections.Generic;
using _Project._200_Dev.Utilities.Extensions;
using DG.Tweening;
using Sirenix.OdinInspector;
using UnityEngine;
using UnityEngine.Events;

namespace _Project._200_Dev.UI
{
    [Serializable]
    public class Page
    {
        [ReadOnly] public string gameObjectName;
        [ReadOnly] public CanvasGroup canvasGroup;
        public int pageIndex;
        public UnityEvent onPageSelectedEvent = new UnityEvent();
        public UnityEvent onPageUnSelectedEvent = new UnityEvent();

        public Page(string gameObjectName, CanvasGroup canvasGroup, int pageIndex)
        {
            this.gameObjectName = gameObjectName;
            this.canvasGroup = canvasGroup;
            this.pageIndex = pageIndex;
        }
    }


    public class Pager : MonoBehaviour
    {
        private RectTransform _rectTransform;

        public List<Page> _pages = new List<Page>();
        [SerializeField, PropertyRange(0, "@_pages.Count")] private int _startupPage;
        [SerializeField, ReadOnly] private int _currentPageIndex;
        [SerializeField, ReadOnly] private int _previousPageIndex;
        [SerializeField] private bool pageLoop = true;
        [SerializeField] private bool _disablePagesGameObject = true;
        [SerializeField] private bool crossFadePages = true;
        [SerializeField] private float _crossFadeDuration = 0.25f;
        private Tween _currentPageFadeTween;
        private Tween _previousPageFadeTween;


        private void Start()
        {
            GoToPage(_startupPage, true);
        }

        [Button]
        public void NextPage()
        {
            if (_currentPageIndex + 1 > _pages.Count - 1)
            {
                if (pageLoop)
                {
                    _previousPageIndex = _currentPageIndex;
                    _currentPageIndex = 0;
                }
                else
                {
                    return;
                }
            }
            else
            {
                _previousPageIndex = _currentPageIndex;
                _currentPageIndex++;
            }

            if (crossFadePages)
            {
                CrossFadePages();
            }
            else ChangePageVisualBehaviour(); 
        }

        [Button]
        public void PreviousPage()
        {
            if (_currentPageIndex - 1 < 0)
            {
                if (pageLoop)
                {
                    _previousPageIndex = _currentPageIndex;
                    _currentPageIndex = _pages.Count - 1;
                }
                else
                {
                    return;
                }
            }
            else
            {
                _previousPageIndex = _currentPageIndex;
                _currentPageIndex--;
            }

            if (crossFadePages)
            {
                CrossFadePages();
            }
            else ChangePageVisualBehaviour();
        }

        [Button]
        public void RefreshPager()
        {
            Debug.Log("Refreshing the pager...");
            _pages.Clear();

            if (_rectTransform == null)
            {
                _rectTransform = GetComponent<RectTransform>();
            }

            List<RectTransform> children =
                _rectTransform.GetComponentsInChildrenFirstDepthWithoutTheParent<RectTransform>();
            for (int i = 0; i < children.Count; i++)
            {
                if (children[i].TryGetComponent(out CanvasGroup canvasGroup))
                {
                    _pages.Add(new Page(canvasGroup.name, canvasGroup, i));
                }
                else
                {
                    CanvasGroup childrenCanvasGroup = children[i].gameObject.AddComponent<CanvasGroup>();
                    _pages.Add(new Page(childrenCanvasGroup.name, childrenCanvasGroup, i));
                }
            }
            
            Debug.Log("Pager refreshed !");
        }

        public void GoToPage(CanvasGroup canvasGroup)
        {
            if (GetPage(_currentPageIndex).canvasGroup == canvasGroup) return;

            for (int i = 0; i < _pages.Count; i++)
            {
                if (GetPage(i).canvasGroup == canvasGroup)
                {
                    GoToPage(i);
                    break;
                }
            }
        }

        public void GoToPage(int pageIndex) => GoToPage(pageIndex, false);

        public void GoToPage(int pageIndex, bool forceGoToPage)
        {
            if (GetPage(pageIndex).pageIndex == _currentPageIndex && forceGoToPage == false) return;

            for (int i = 0; i < _pages.Count; i++)
            {
                if (GetPage(i).pageIndex == pageIndex)
                {
                    _previousPageIndex = _currentPageIndex;
                    _currentPageIndex = i;

                    GetPage(_previousPageIndex).onPageUnSelectedEvent.Invoke();
                    GetPage(_currentPageIndex).onPageSelectedEvent.Invoke();
                    
                    if (crossFadePages)
                    {
                        CrossFadePages();
                    }
                    else ChangePageVisualBehaviour();
                    break;
                }
            }
        }
        
        private Page GetPage(int pageIndex) => _pages[pageIndex];

        private void CrossFadePages()
        {
            #if UNITY_EDITOR
            // Dotween dont work in editor
            if (Application.isPlaying == false)
            {
                ChangePageVisualBehaviour();
                return;
            }
            #endif
            
            _previousPageFadeTween?.Kill(true);
            _currentPageFadeTween?.Kill(true);
            
            CanvasGroup previousPageCanvasGroup = GetPage(_previousPageIndex).canvasGroup;
            CanvasGroup currentCanvasGroup = GetPage(_currentPageIndex).canvasGroup;

            _previousPageFadeTween = previousPageCanvasGroup.DOFade(0.0f, _crossFadeDuration).OnComplete(() => _previousPageFadeTween = null);
            _currentPageFadeTween = currentCanvasGroup.DOFade(1.0f, _crossFadeDuration).OnComplete(() =>
            {
                ChangePageVisualBehaviour();
                _currentPageFadeTween = null;
            });;
        }

        private void ChangePageVisualBehaviour()
        {
            if (_disablePagesGameObject)
            {
                _pages[_previousPageIndex].canvasGroup.gameObject.SetActive(false);
                _pages[_currentPageIndex].canvasGroup.gameObject.SetActive(true);
            }
            else
            {
                _pages[_previousPageIndex].canvasGroup.alpha = 0.0f;
                _pages[_previousPageIndex].canvasGroup.blocksRaycasts = false;
                
                _pages[_currentPageIndex].canvasGroup.alpha = 1.0f;
                _pages[_currentPageIndex].canvasGroup.blocksRaycasts = true;
            }
        }
    }
}