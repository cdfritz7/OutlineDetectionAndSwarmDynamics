import cv2
import numpy as np
from keras.preprocessing.image import img_to_array
from data_training import get_training_data
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten
from keras.layers import Conv2D, MaxPooling2D
from sklearn.model_selection import train_test_split

def get_model():
    data_train = get_training_data()
    X_train, X_test, y_train, y_test = train_test_split(data_train[0], data_train[1], random_state=42, test_size=0.2)
    model = Sequential()
    model.add(Conv2D(32, kernel_size=(3, 3),activation='relu',input_shape=(50,50,1)))
    model.add(Conv2D(64, (3, 3), activation='relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Dropout(0.25))
    model.add(Flatten())
    model.add(Dense(128, activation='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(3, activation='softmax'))

    model.compile(loss='categorical_crossentropy',optimizer='Adam',metrics=['accuracy'])
    model.fit(X_train, y_train, epochs=10, validation_data=(X_test, y_test))

    return model

model = None
cap = cv2.VideoCapture(0)

while (cap.isOpened()):

    try:
        ret, frame = cap.read() #ret: true or false #frame: size
        frame = cv2.flip(frame, 1) #flip the frame
        dim = (50, 50)
        resized_image = cv2.resize(frame, dim, interpolation=cv2.INTER_AREA)

        hsv = cv2.cvtColor(resized_image, cv2.COLOR_BGR2HSV)

        lower_skin = np.array([0, 20, 70], dtype='float32')
        upper_skin = np.array([20, 255, 255], dtype='float32')

        mask = cv2.inRange(hsv, lower_skin, upper_skin)
        cv2.imshow('mask', mask)
        img_reverted = img_to_array(mask) / 255
        #print(img_reverted)

        if model is None:
            model = get_model()
        X_test = []
        X_test.append(img_reverted)
        X = np.asarray(X_test)
        prediction = model.predict(X)
        print(prediction)

        #cv2.imshow('frame', frame)

    except:
        pass

    k = cv2.waitKey(5) & 0xFF
    if k == 27:  # quit if 'q' is pressed
        break

cv2.destroyAllWindows()
cap.release()
