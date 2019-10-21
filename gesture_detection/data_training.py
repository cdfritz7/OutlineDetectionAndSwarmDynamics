import cv2
import numpy as np
from keras.utils import to_categorical
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten
from keras.layers import Conv2D, MaxPooling2D
from keras.preprocessing.image import img_to_array
from sklearn.model_selection import train_test_split

def process_image(name):
    try:
        image = cv2.imread(filename= name)
        dim = (50, 50)
        resized_image = cv2.resize(image, dim, interpolation=cv2.INTER_AREA)


        hsv = cv2.cvtColor(resized_image, cv2.COLOR_BGR2HSV)

        lower_skin = np.array([0, 20, 70], dtype='float32')
        upper_skin = np.array([20, 255, 255], dtype='float32')

        mask = cv2.inRange(hsv, lower_skin, upper_skin)

        img_reverted =img_to_array(mask)/255
        #print(img_reverted)
        #print(img_reverted.shape)
        # black = 1, white = 0

        #cv2.imshow("resized gray image", mask)
        #cv2.imwrite(name, mask)
        #cv2.waitKey(0)
        #cv2.destroyAllWindows()

        return img_reverted
    except Exception as e:
        print(str(e))



def get_training_data():
    palm_path = 'images/palm/palm '
    peace_path = 'images/peace/peace '
    X_train = []
    y_train = []
    for i in range(1,14):

        palm_name = palm_path + "(" + str(i) + ").jpg"
        peace_name = peace_path + "(" + str(i) + ").jpg"
        processed_palm = process_image(palm_name)
        processed_peace = process_image(peace_name)
        X_train.append(processed_palm)
        X_train.append(processed_peace)

        y_train.append(0)
        y_train.append(1)

    X = np.asarray(X_train)
    print(y_train)
    y = np.asarray(to_categorical(y_train))
    print(y)
    data_train = [X,y]
    return data_train

def get_test_data():
    path = 'images/test/test'
    X_test = []
    for i in range(1,5):
        name = path + str(i) + ".jpg"
        processed_test = process_image(name)
        X_test.append(processed_test)

    X = np.asarray(X_test)
    return X


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
model.add(Dense(2, activation='softmax'))

print(X_train.shape)
print(X_test.shape)
print(y_train)
print(y_test)


model.compile(loss='categorical_crossentropy',optimizer='Adam',metrics=['accuracy'])
model.fit(X_train, y_train, epochs=10, validation_data=(X_test, y_test))

#prediction = model.predict_classes(X_train)
#print(prediction)
