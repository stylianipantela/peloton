//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// abstract_plan.h
//
// Identification: src/include/planner/abstract_plan.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <iostream>
#include <memory>
#include <cstdint>
#include <vector>
#include <map>
#include <vector>

#include "common/printable.h"
#include "common/types.h"
#include "common/value.h"
#include "common/serializer.h"
#include "common/serializeio.h"
#include "catalog/schema.h"

namespace peloton {

namespace executor {
class AbstractExecutor;
class LogicalTile;
}

namespace catalog {
class Schema;
}

namespace expression {
class AbstractExpression;
}

namespace planner {

//===--------------------------------------------------------------------===//
// Abstract Plan
//===--------------------------------------------------------------------===//

class AbstractPlan : public Printable {
 public:
  AbstractPlan(const AbstractPlan &) = delete;
  AbstractPlan &operator=(const AbstractPlan &) = delete;
  AbstractPlan(AbstractPlan &&) = delete;
  AbstractPlan &operator=(AbstractPlan &&) = delete;

  AbstractPlan();

  virtual ~AbstractPlan();

  //===--------------------------------------------------------------------===//
  // Children + Parent Helpers
  //===--------------------------------------------------------------------===//

  void AddChild(std::unique_ptr<AbstractPlan> &&child);

  const std::vector<std::unique_ptr<AbstractPlan>> &GetChildren() const;

  const AbstractPlan *GetParent();

  //===--------------------------------------------------------------------===//
  // Accessors
  //===--------------------------------------------------------------------===//

  // Each sub-class will have to implement this function to return their type
  // This is better than having to store redundant types in all the objects
  virtual PlanNodeType GetPlanNodeType() const = 0;

  // Setting values of the parameters in the prepare statement
  virtual void SetParameterValues(std::vector<common::Value *> *values) = 0;

  //===--------------------------------------------------------------------===//
  // Utilities
  //===--------------------------------------------------------------------===//

  // Get a string representation for debugging
  const std::string GetInfo() const;

  virtual std::unique_ptr<AbstractPlan> Copy() const = 0;

  // A plan will be sent to anther node via serialization
  // So serialization should be implemented by the derived classes

  //===--------------------------------------------------------------------===//
  // Serialization/Deserialization
  // Each sub-class will have to implement these functions
  // After the implementation for each sub-class, we should set these to pure
  // virtual
  //===--------------------------------------------------------------------===//
  virtual bool SerializeTo(SerializeOutput &output UNUSED_ATTRIBUTE) const {
    PL_ASSERT(&output != nullptr);
    return false;
  }
  virtual bool DeserializeFrom(SerializeInput &input UNUSED_ATTRIBUTE) {
    PL_ASSERT(&input != nullptr);
    return false;
  }
  virtual int SerializeSize() { return 0; }

 protected:
  // only used by its derived classes (when deserialization)
  AbstractPlan *Parent() { return parent_; }

  // Convert COLUMN_REF in AbstractExpression to TupleValueExpression
  void ReplaceColumnExpressions(catalog::Schema *schema,
                                expression::AbstractExpression *expression);

 private:
  // A plan node can have multiple children
  std::vector<std::unique_ptr<AbstractPlan>> children_;

  AbstractPlan *parent_ = nullptr;
};

}  // namespace planner
}  // namespace peloton
