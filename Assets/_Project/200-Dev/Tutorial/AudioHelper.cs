using Cysharp.Threading.Tasks;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using UnityEngine;


namespace Project
{
    public class AudioHelper : MonoBehaviour
    {
            public AudioHelperItem CurrentlyPlaying { get; private set; }
            public List<AudioHelperItem> Items = new List<AudioHelperItem>();

            public float RemainingTime
            {
                get
                {
                    return Mathf.Clamp((float)(currentClipEndTime - DateTime.Now).TotalSeconds, 0, float.MaxValue);
                }
            }

            public float CurrentClipTime
            {
                get
                {
                    return Mathf.Clamp((float)(DateTime.Now - currentClipStartTime).TotalSeconds, 0, float.MaxValue);
                }
            }

            private DateTime currentClipEndTime;
            private DateTime currentClipStartTime; 

            public delegate void AudioPlayerTimeStampMessage(string msg);
            public static event AudioPlayerTimeStampMessage OnTimestampReached;


            public async UniTask PlayAsync(string eventName, CancellationToken ct)
            {

                var emitter = Items.FirstOrDefault(x => x.eventName == eventName);

            try
            {
                if (emitter != null) {

                    if (CurrentlyPlaying != null)
                    {
                        if (CurrentlyPlaying.eventEmitter.IsPlaying())
                        {
                            CurrentlyPlaying.eventEmitter.Stop();
                        }
                    }

                    emitter.eventEmitter.Play();
                    emitter.eventEmitter.EventDescription.getLength(out int len);

                    currentClipStartTime = DateTime.Now;
                    currentClipEndTime = DateTime.Now.AddMilliseconds(len);

                    CurrentlyPlaying = emitter;

                    CancellationTokenSource ctsMonitor = new CancellationTokenSource(len);
                    MonitorTimeStampMessagesAsync(ctsMonitor.Token);

                    await UniTask.Delay(len, cancellationToken: ct);
                    CurrentlyPlaying = null;


                }
            }
            catch (Exception ex)
            {
                CurrentlyPlaying?.eventEmitter.Stop();
                Debug.LogError("Je déteste FMOD " + ex.Message);
            }
                
            }

        public async UniTask<bool> MonitorTimeStampMessagesAsync(CancellationToken ct)
        {
            var items = CurrentlyPlaying.timeStamps.OrderBy(x => x.Timestamp).ToDictionary(t => t.Timestamp);

            try
            {
                foreach (var i in items)
                {
                    await UniTask.WaitUntil(() => CurrentClipTime > i.Value.Timestamp, cancellationToken: ct);
                    if (OnTimestampReached != null)
                        OnTimestampReached(i.Value.Message);
                }
            }
            catch (Exception e)
            {
                return false;
            }
            return true;
        }


    }
}