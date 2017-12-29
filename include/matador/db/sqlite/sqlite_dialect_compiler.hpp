//
// Created by sascha on 17.06.16.
//

#ifndef OOS_SQLITE_DIALECT_COMPILER_HPP
#define OOS_SQLITE_DIALECT_COMPILER_HPP

#include "matador/sql/dialect_token.hpp"

#include "matador/sql/basic_dialect_compiler.hpp"

namespace matador {

namespace sqlite {

class sqlite_dialect;

class sqlite_dialect_compiler : public detail::basic_dialect_compiler
{
public:
  virtual void visit(const matador::detail::select &select1) override;
  virtual void visit(const matador::detail::update &update1) override;
  virtual void visit(const matador::detail::remove &remove1) override;
  virtual void visit(const matador::detail::tablename &tab) override;
  virtual void visit(const matador::detail::from &from1) override;

  virtual void visit(const matador::detail::where &where1) override;
  virtual void visit(const matador::detail::top &top1) override;

protected:
  virtual void on_compile_start() override;

private:
  bool is_update = false;
  bool is_delete = false;

  std::string tablename_;
  token_list_t::iterator where_;
};

}
}

#endif //OOS_SQLITE_DIALECT_COMPILER_HPP
