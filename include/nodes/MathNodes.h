#pragma once
#include "DecimalData.h"
#include "ErrorData.h"
#include "MathOperationDataModel.h"

class AdditionModel : public MathOperationDataModel {
public:
  QString caption() const override { return "Addition"; }
  QString name() const override { return "Addition"; }

protected:
  void compute() override {
    executePolymorphic([](double a, double b) { return a + b; });
  }
};

class SubtractionModel : public MathOperationDataModel {
public:
  QString caption() const override { return "Subtraction"; }
  QString name() const override { return "Subtraction"; }

protected:
  void compute() override {
    executePolymorphic([](double a, double b) { return a - b; });
  }
};

class MultiplicationModel : public MathOperationDataModel {
public:
  QString caption() const override { return "Multiplication"; }
  QString name() const override { return "Multiplication"; }

protected:
  void compute() override {
    executePolymorphic([](double a, double b) { return a * b; });
  }
};

class DivisionModel : public MathOperationDataModel {
public:
  QString caption() const override { return "Division"; }
  QString name() const override { return "Division"; }

protected:
  void compute() override {
    // Basic division for now, advanced error handling for arrays later
    executePolymorphic([](double a, double b) {
      return (b != 0.0) ? a / b : 0.0; // Naive zero check
    });
  }
};
