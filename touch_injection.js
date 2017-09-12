function sendTouchEvent(touchDataObjStr) {
    var touchDataObj = JSON.parse(touchDataObjStr);
    var touchDataArray = touchDataObj.touchArray;
    var eventType = touchDataObj.event;
    var touchArray = [];
    for (var i in touchDataArray) {
        const touchObj = new Touch({
          target: document,
          clientX: touchDataArray[i].x,
          clientY: touchDataArray[i].y,
          radiusX: 2.5,
          radiusY: 2.5,
          rotationAngle: 10,
          force: 0.5,
          identifier: touchDataArray[i].id
        });
        touchArray.push(touchObj);
    }

  const touchEvent = new TouchEvent(eventType, {
    cancelable: true,
    bubbles: true,
    touches: touchArray,
    targetTouches: [],
    changedTouches: touchArray,
    shiftKey: true,
    isTrusted : true
  });

  document.dispatchEvent(touchEvent);
}

//function sendTouchEvent(x, y,id, eventType) {
//  var element = document.elementFromPoint( x, y );
//  const touchObj = new Touch({
//    identifier: Date.now(),
//    target: element,
//    clientX: x,
//    clientY: y,
//    radiusX: 2.5,
//    radiusY: 2.5,
//    rotationAngle: 10,
//    force: 0.5,
//    identifier: id
//  });

//  const touchEvent = new TouchEvent(eventType, {
//    cancelable: true,
//    bubbles: true,
//    touches: [touchObj],
//    targetTouches: [],
//    changedTouches: [touchObj],
//    shiftKey: true,
//    isTrusted : true
//  });

//  element.dispatchEvent(touchEvent);
//}
