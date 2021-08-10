#include <iostream>
#include <sstream>

int electrical = 0;

enum electricalPanel {
	HOME_POWER_SUPPLY = 1 << 0,
	SOCKET_HOUSE      = 1 << 1,
	LIGHT_IN_HOUSE    = 1 << 2,
	LIGHT_ON_GARDEN   = 1 << 3,
	HEATING_HOUSE     = 1 << 4,
	HEATING_WATER     = 1 << 5,
	AIR_CONDITIONER   = 1 << 6
};

bool convertSensor(std::string sensor) {
  return (sensor == "yes" || sensor == "on");
}

void checkTemperatureLight(int hour) {
	int startTime = 16;
	int endTime = 20;
	int hightTemp = 5000;
	int lowTemp = 2700;

	std::cout << "••• Цветовая температура в доме: ";
	if (hour < startTime)
		std::cout << hightTemp << 'K' << std::endl;
	else
		(hour < endTime) ? std::cout << hightTemp - ((hightTemp - lowTemp) / (endTime - startTime) * (hour - startTime)) << "K\n" : std::cout << lowTemp << "K\n";
}

void checkTempIn(int sensorTempIn) {
  if ((sensorTempIn < 22) & ((electrical & HEATING_HOUSE) != HEATING_HOUSE)) {
		electrical |= HEATING_HOUSE;
		std::cout << "••• Отопление в доме: Включёно\n";
	}
	else {
		if ((sensorTempIn >= 25) & ((electrical & HEATING_HOUSE) == HEATING_HOUSE)) {
			electrical &= ~HEATING_HOUSE;
			std::cout << "••• Отопление в доме: Выключено\n";
		}
  }
}

void checkAirConditioner(int sensorTempIn) {
  if ((sensorTempIn >= 30) & ((electrical & AIR_CONDITIONER) != AIR_CONDITIONER)) {
		electrical |= AIR_CONDITIONER;
		std::cout << "••• Кондиционер: Включён\n";
	}
	else {
		if ((sensorTempIn <= 25) & ((electrical & AIR_CONDITIONER) == AIR_CONDITIONER)) {
			electrical &= ~AIR_CONDITIONER;
			std::cout << "••• Кондиционер: Выключен\n";
		}
  }
}

void checkTempOut(int sensorTempOut) {
  if ((sensorTempOut < 0) & ((electrical & HEATING_WATER) != HEATING_WATER)) {
		electrical |= HEATING_WATER;
		std::cout << "••• Отопление котла: Включёно\n";
	}
	else {
		if ((sensorTempOut > 5) & ((electrical & HEATING_WATER) == HEATING_WATER)) {
			electrical &= ~HEATING_WATER;
			std::cout << "••• Отопление котла: Выключено\n";
		}
  }
}

void checkLightOnGarden(int hour, bool sensorMotion) {
  if (((hour < 5 || hour > 16) && sensorMotion) & ((electrical & LIGHT_ON_GARDEN) != LIGHT_ON_GARDEN)) {
		electrical |= LIGHT_ON_GARDEN;
		std::cout << "••• Свет в саду: Включён\n";
	}
	else {
		if (((hour >= 5 && hour <= 16) || !sensorMotion) & ((electrical & LIGHT_ON_GARDEN) == LIGHT_ON_GARDEN)) {
			electrical &= ~LIGHT_ON_GARDEN;
			std::cout << "••• Свет в саду: Выключен\n";
		}
  }
}

void checkLightInHouse(bool sensorLight) {
  if ((sensorLight) && ((electrical & LIGHT_IN_HOUSE) != LIGHT_IN_HOUSE)) {
		electrical |= LIGHT_IN_HOUSE;
		std::cout << "••• Свет в доме: Включён\n";
	}
	else {
		if ((electrical & LIGHT_IN_HOUSE) == LIGHT_IN_HOUSE) {
			electrical &= ~LIGHT_IN_HOUSE;
			std::cout << "••• Свет в доме: Выключен\n";
		}
  }
}


int main() {
  int totalDays = 2, sensorTempOut, sensorTempIn;
  bool sensorMotion, sensorLight;
  std::string indicators, sensorMotionTmp, sensorLightTmp;
	std::stringstream sstream;

  for (int day = 1; day <= totalDays; ++day) {
    for (int hour = 0; hour < 24; ++hour) {
      std::cout << "\nДень - " << day << ". Время - ";
      std::cout << ((hour < 10) ? ("0") : (""));
      std::cout << hour << ":00\nВведите показатели: t снаружи, t внутри, движение, свет: \n";
      getline(std::cin, indicators);
      sstream << indicators;
      sstream >> sensorTempIn >> sensorTempOut >> sensorMotionTmp >> sensorLightTmp;
      sensorMotion = convertSensor(sensorMotionTmp);
      sensorLight = convertSensor(sensorLightTmp);
      
      checkTempIn(sensorTempIn);
      checkAirConditioner(sensorTempIn);
      checkTempOut(sensorTempOut);
      checkLightOnGarden(hour, sensorMotion);
      checkLightInHouse(sensorLight);

      if (sensorLight)
			  checkTemperatureLight(hour);
    }
  }
}
/*
Умный дом
Что нужно сделать
Надо разработать упрощённую модель умного дома для дачи. Есть несколько датчиков: датчик температуры снаружи, датчик температуры внутри дома. В доме расположен электронный “умный” щиток с автоматами. Каждый автомат отвечает за включение и отключение определённых устройств или групп устройств. Первый автомат - это отключение и включение питания всего дома, второй - основные розетки дома, третий - свет внутри дома, четвёртый - наружный свет в саду, пятый - отопление в помещении, шестой - отопление водопровода идущего из скважины с насосом, шестой - кондиционер в помещении.
Есть несколько условий по включению/отключению техники в доме и вне его.
Как только температура снаружи падает ниже 0, надо включить систему обогрева водопровода и отключить её, если температура снаружи поднялась выше 5 градусов.
Если на дворе вечер (время больше 16:00 и меньше 5:00 утра) и снаружи есть какое-то движение, то необходимо включить садовое освещение. Если движения нет или время не вечернее - света снаружи быть не должно.
При условии, что температура в помещении упала ниже 22 градусов, должно включится отопление. Как только температура поднимается выше или равно 25-и градусам, отопление автоматически отключается.
Если температура в помещении поднялась до 30 градусов, включается кондиционер. Как только температура становится 25 градусов - кондиционер отключается.
Всё освещение в доме также “умное” и поддерживает настройку цветовой температуры для комфортного нахождения. Каждый день начиная с 16:00 и до 20:00, температура цвета должна плавно изменяться с 5000K до 2700К. Разумеется, это изменение должно происходить в случае, если свет сейчас включен. В 0:00 температура сбрасывается до 5000К.
Все события по отключению и включению устройств должны выводится в консоль явным образом. Если устройство не изменило своего состояния (осталось включенным или выключенным) события генерироваться не должно! Если свет в доме включен, должна отображаться текущая цветовая температура.
Программа выполняется следующим образом. Каждый час пользователь сообщает состояние всех основных датчиков и света: температура снаружи, температура внутри, есть ли движение снаружи, включен ли свет в доме. Данные параметры вводятся разом в одну строку, через пробел, а потом парсятся в переменные из строкового буфера stringstream. Информация о движении выводится в формате yes/no. Включение и отключение света происходит с помощью on/off. Стартовое время для симуляции умного дома - это 0:00. Требуется осуществить симуляцию на протяжении двух дней.
*/